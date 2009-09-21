/*
 * Copyright  1990-2008 Sun Microsystems, Inc. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 only, as published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included at /legal/license.txt).
 * 
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 * Clara, CA 95054 or visit www.sun.com if you need additional
 * information or have any questions.
 */

/**
 * @file
 * @brief Content Handler Registry stubs.
 */

#include "javacall_chapi_invoke.h"
#include "javacall_chapi_registry.h"
#include "inc/javautil_storage.h"

#define MALLOC malloc
#define REALLOC realloc
#define FREE free

#define MAX_BUFFER 512
#define MAX_LINE 1024
#define XJAVA_ARRAY_DIVIDER ':'

#define TYPE_INFO_JAVA_HANDLER 0x10

#define TYPE_INFO_USE_REFERENCE 0x100

#define TYPE_INFO_ACTION_MASK 0x2F000000

#define TYPE_INFO_ACTION_DEFAULT 0x20000000

typedef struct _content_type_info{
	short* type_name;
	short* description;
	short* nametemplate;
	short** suffixes;
	int flag;
	int actions_refcount;
} content_type_info;

typedef struct _handler_info{
	int flag;
	short* handler_id;
	short* handler_friendly_name;
	union {
		short* classname;
		short* appname;
	};
	short* suite_id;
	int jflag;
	short** access_list;
} handler_info;

typedef struct _action_info{
	content_type_info* content_type;
	handler_info* handler;
	int flag;
	union {
		short* actionname;
		const short* actionname_const;
	};
	short* params;
	short** locales;
	short** localnames;
} action_info;


#define INFO_INC 128

#define CHAPI_INDEX 0
#define ACTIONMAP_INDEX 1
#define ACCESSLIST_INDEX 2

unsigned long chapi_lastread = 0;
unsigned long actionmap_lastread = 0;
unsigned long accesslist_lastread = 0;

char* chapi_fname = "chapi";
char* actionmap_fname = "actionmap";
char* accesslist_fname = "accesslist";

const char* java_invoker = "${JVM_INVOKER}";
const unsigned short* java_type = L"application/x-java-content";

content_type_info** g_content_type_infos = 0;
int g_content_type_infos_used;
int g_content_type_infos_allocated;


handler_info** g_handler_infos = 0;
int g_handler_infos_used;
int g_handler_infos_allocated;

action_info** g_action_infos = 0;
int g_action_infos_used;
int g_action_infos_allocated;

const short* DEFAULT_ACTION = 0;

#define CHAPI_READ 1
#define CHAPI_WRITE 2
#define CHAPI_APPEND 3

#define chrieq(a,b) (((a ^ b) & ~0x20) == 0)

static int open_db(int db_index, javautil_storage* file, int flag);
static void close_db(javautil_storage file);
static void free_action_info(action_info* info);
static int read_access_list();
static int read_action_map();
static int read_caps();
static int is_modified(int index);
static void update_lastread(int index);
static void reset_lastread();
static javacall_bool is_access_allowed( handler_info* info, javacall_const_utf16_string caller_id );

/******************* Type Infos **********************/

static content_type_info* new_content_type_info(short* type_name){
	content_type_info* info;
	if (!g_content_type_infos){
		g_content_type_infos = (content_type_info**)MALLOC(sizeof(content_type_info*)*INFO_INC);
		if (!g_content_type_infos) return 0;
		g_content_type_infos_allocated = INFO_INC;
		g_content_type_infos_used = 0;
	}
	if (g_content_type_infos_used>=g_content_type_infos_allocated){
		content_type_info** tmp = (content_type_info**)REALLOC(g_content_type_infos,sizeof(content_type_info*)*(g_content_type_infos_allocated+INFO_INC));
		if (!tmp) return 0;
		g_content_type_infos = tmp;
		g_content_type_infos_allocated += INFO_INC;
	}
	info = (content_type_info*)MALLOC(sizeof(content_type_info));
	if (!info) return 0;
	memset(info,0,sizeof(content_type_info));
	g_content_type_infos[g_content_type_infos_used++] = info;
	info->type_name = type_name;
	return info;
}

static void free_list(void** list){
	void **p;
	if (p=list){
		while (*p) FREE(*p++);
		FREE(list);
	}
}

static void free_content_type_info(content_type_info* info){
	if (info->type_name) FREE(info->type_name);
	if (info->description) FREE(info->description);
	if (info->nametemplate) FREE(info->nametemplate);
	free_list((void**)info->suffixes);
	FREE(info);
}

static void release_content_type_info(content_type_info* type){
	if (!--type->actions_refcount){
		int index=g_content_type_infos_used;
		while (index-- && g_content_type_infos[index]!=type);
		if (index>=0 && index<g_content_type_infos_used-1) 
			memmove(&g_content_type_infos[index],&g_content_type_infos[index+1],g_content_type_infos_used-1-index);
		free_content_type_info(type);
		g_content_type_infos_used--;
	}
}


/******************* Handler Infos **********************/

static handler_info* new_handler_info(short* handler_id){
	handler_info* info;
	if (!g_handler_infos){
		g_handler_infos = (handler_info**)MALLOC(sizeof(handler_info*)*INFO_INC);
		if (!g_handler_infos) return 0;
		g_handler_infos_allocated = INFO_INC;
		g_handler_infos_used = 0;
	}
	if (g_handler_infos_used>=g_handler_infos_allocated){
		handler_info** tmp = (handler_info**)REALLOC(g_handler_infos,sizeof(handler_info*)*(g_handler_infos_allocated+INFO_INC));
		if (!tmp) return 0;
		g_handler_infos = tmp;
		g_handler_infos_allocated += INFO_INC;
	}
	info = (handler_info*)MALLOC(sizeof(handler_info));
	if (!info) return 0;
	memset(info,0,sizeof(handler_info));
	info->handler_id = handler_id;
	g_handler_infos[g_handler_infos_used++] = info;
	return info;
}

static void free_handler_info(handler_info* info){
	if (info->handler_id) FREE(info->handler_id);
	if (info->classname) FREE(info->classname);
	if (info->suite_id) FREE(info->suite_id);
	if (info->flag & TYPE_INFO_JAVA_HANDLER){
		if (info->handler_friendly_name) FREE(info->handler_friendly_name);
		//on not-java handlers handler_friendly_name refers to appname
	}
	if (info->access_list) free_list((void**)info->access_list);
	FREE(info);
}

static void delete_handler_info(int index){
	if (g_handler_infos && g_handler_infos_used<index) {
		int i,j;
		handler_info* info = g_handler_infos[index];
		//delete actions
		if (g_action_infos){
			for (i = j =0 ; i<g_action_infos_used; ++i,++j){
				if (g_action_infos[i]->handler == info){
					if (g_action_infos[i]->content_type) release_content_type_info(g_action_infos[i]->content_type);
					free_action_info(g_action_infos[i]);
					--j;
				} else {
					if (i!=j) g_action_infos[j]=g_action_infos[i];
				}
			}
			g_action_infos_used = j;
		}
		//delete info
		free_handler_info(info);
		if (index<--g_handler_infos_used) 
			memmove(&g_handler_infos[index],&g_handler_infos[index+1],g_handler_infos_used-1-index);
	}
}

static void pop_handler_info(){
	if (g_handler_infos_used) delete_handler_info(g_handler_infos_used-1);
}

/******************* Actions **********************/

static action_info* new_action_info(content_type_info* type, handler_info* handler){
	action_info* info;
	if (!g_action_infos){
		g_action_infos = (action_info**)MALLOC(sizeof(action_info*)*INFO_INC);
		if (!g_action_infos) return 0;
		g_action_infos_allocated = INFO_INC;
		g_action_infos_used = 0;
	}
	if (g_action_infos_used>=g_action_infos_allocated){
		action_info** tmp = (action_info**)REALLOC(g_action_infos,sizeof(action_info*)*(g_action_infos_allocated+INFO_INC));
		if (!tmp) return 0;
		g_action_infos = tmp;
		g_action_infos_allocated += INFO_INC;
	}
	info = (action_info*)MALLOC(sizeof(action_info));
	if (!info) return 0;
	memset(info,0,sizeof(action_info));
	info->content_type = type;
	info->handler = handler;
	g_action_infos[g_action_infos_used++] = info;
	type->actions_refcount++; // add ref
	return info;
}

static void free_action_info(action_info* info){
	if (info->actionname && !(info->flag & TYPE_INFO_ACTION_MASK)) {
		FREE(info->actionname);
	}
	if (info->params) {
		FREE(info->params);
	}
	if (!(info->flag & TYPE_INFO_USE_REFERENCE)){
		free_list((void**)info->locales);
		free_list((void**)info->localnames);
	}
	FREE(info);
}

/******************* registry utils **********************/

static void clean_registry(){
	if (g_content_type_infos){
		while (g_content_type_infos_used){
			free_content_type_info(g_content_type_infos[--g_content_type_infos_used]);
		}
		FREE(g_content_type_infos);
		g_content_type_infos = 0;
		g_content_type_infos_allocated = 0;
	}

	if (g_handler_infos){
		while (g_handler_infos_used){
			free_handler_info(g_handler_infos[--g_handler_infos_used]);
		}
		FREE(g_handler_infos);
		g_handler_infos = 0;
		g_handler_infos_allocated = 0;
	}

	if (g_action_infos){
		while (g_action_infos_used){
			free_action_info(g_action_infos[--g_action_infos_used]);
		}
		FREE(g_action_infos);
		g_action_infos = 0;
		g_action_infos_allocated = 0;
	}
}

static int update_registry(){
	int res=0;
	if (is_modified(CHAPI_INDEX)){
		clean_registry();
		return read_caps();
	}
	if (g_handler_infos_used && is_modified(ACCESSLIST_INDEX)){
		res=read_access_list();
		if (res) return res;
	}
	if (g_action_infos_used && is_modified(ACTIONMAP_INDEX)){
		res=read_action_map();
	}

	return res;
}


/******************* common utils **********************/

static int copy_string(const short* str, /*OUT*/ unsigned short*  buffer, int* length){
	int len = (str?javautil_str_wcslen(str):0)+1;
	if (!length || (*length && !buffer)) return JAVACALL_CHAPI_ERROR_BAD_PARAMS;

	if (*length < len){
		*length = len;
		return JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL;
	}
	if (len>1) {
		memcpy(buffer,str,len*sizeof(*str));
	} else {
		buffer[0]=0;
	}

	*length = len;
	return 0;
}

static short unhex(short code){
	if (code >= '0' &&  code <= '9') return code - '0';
	if (code >= 'A' &&  code <= 'F') return code - 'A' + 0xa;
	if (code >= 'a' &&  code <= 'f') return code - 'a' + 0xa;
	return -1;
}

// encode unicode string to acsii escape string and return length of encoded string
static int append_string(char* buffer, const unsigned short* str){
	const char tohex[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	char* buf = buffer;
	--str;--buf;
	while (*++str){
		if ((*str & 0xFF) == *str && *str != '%' && *str != '\'' && *str != ';') {
			*++buf = (char)(*str & 0xFF);
		} else {
			//unicode to ascii
			*++buf = '%';
			*++buf = tohex[(*str >> 12) & 0xF];
			*++buf = tohex[(*str >> 8) & 0xF];
			*++buf = '%';
			*++buf = tohex[(*str >> 4) & 0xF];
			*++buf = tohex[(*str) & 0xF];
		}
	}

	*++buf = '\0';
	return buf-buffer;
}


/******************* CHAPI parsing **********************/

/**
*   reads new not empty not commented line from file
*	line and max_size should not be zero
**/
static int get_line(short* line, int max_size, javautil_storage f){
	long pos;
	int i=-1,count=0;
	javautil_storage_getpos(f,&pos);
	while (javautil_storage_read(f,(char*)&i,1)==1){
		i&=0xFF;
		if ((short)i=='#'){
			while (javautil_storage_read(f,(char*)&i,1)==1 && (short)i!='\n');
		}
		if ((short)i!='\n' && (short)i!='\r') break;
	}
	while (i>0) {
		i &= 0xFF;
		if (i == '\n') break;
		if (count>=max_size-1) {
			//buffer too small return
			javautil_storage_setpos(f,pos,JUS_SEEK_SET);
			return JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL;
		}

		if (i == '%'){ //decode Unicode character
			long pos2;
			int code[4];
			char b[5];
			javautil_storage_getpos(f,&pos2);
			if (javautil_storage_read(f,b,5)==5 && b[2]=='%' &&	
				    (code[0]=unhex(b[0])>=0) &&
				    (code[1]=unhex(b[1])>=0) &&
				    (code[2]=unhex(b[3])>=0) &&
				    (code[3]=unhex(b[4])>=0) ) {
				i = (unsigned short) ((code[0]<<12) + (code[1]<<8) + (code[2]<<4) + code[3]);
			} else javautil_storage_setpos(f,pos2,JUS_SEEK_SET);
		}

		line[count++]=(short)i;
		if (javautil_storage_read(f,(char*)&i,1)!=1) break;
	}
	line[count]=0;
	return count;
}

//find next token that ends on ; and can contain key and value separated by =
static int next_key(short* line,short** kstart,short** kend, short** valstart,short** valend, short** tokenend){
	int p=0,quot=0,dquot=0,endword=0;
	while (line[p]==' ' || line[p]=='\t') ++p; //trim left
	if(!line[p]) return 0;
	endword=p;
	if (kstart) *kstart = &line[p];
	if (kend) *kend = &line[p];
	if (valstart) *valstart = 0;
	if (valend) *valend = 0;

	while (line[p]){
		if (line[p]=='=' && !quot && !dquot){ // value found
			if (kend) *kend = &line[endword];
			while (line[++p] == ' '); //trim left
			if (valstart) *valstart = &line[p];
			if (valend) *valend = &line[p];
			endword=p;
			while (line[p]){
				if (line[p]==';' && !quot && !dquot){
					p++;
					break;
				}
				if (line[p]=='\''&& !dquot){ quot = !quot;}
				if (line[p]=='\"'&& !quot){ dquot = !dquot;}
				if (line[p]!=' ' && line[p]!='\t') endword=p; //trim right
				++p;
			}
			if (valend) *valend = &line[endword];
			break;
		}
		if (line[p]==';' && !quot && !dquot){
			if (kend) *kend = &line[endword];
			if (valstart) *valstart = 0;
			if (valend) *valend = 0;
			p++;break;
		}
		if (line[p]=='\'' && !dquot){ quot = !quot;}
		if (line[p]=='\"'&& !quot){ dquot = !dquot;}
		if (line[p]!=' ' && line[p]!='\t') endword=p; //trim right
		++p;
	}
	if (tokenend) *tokenend = &line[p];
	return 1;
}

static int next_key_unquote(short* line,short** kstart,short** kend, short** valstart,short** valend, short** tokenend){
	int res = next_key(line,kstart,kend,valstart,valend, tokenend);
	if (kstart && kend && *kstart && *kend && *kend>*kstart && **kstart=='\'' && **kend=='\'') {
		*kstart = *kstart + 1; *kend = *kend - 1; //unquote
	}
	return res;
}

static int match(const short* p1, const short* pend, const unsigned short* p2){
	if (!p1 || !p2) return 0;
	while (*p1 && (p1<=pend) && *p2 && chrieq(*p1,*p2)){
		++p1;
		++p2;
	}
	return (!*p2);
}

/* search for key in line if found returns -1 and value location, else returns 0 */
static int find_key(short* line, const unsigned short* key, short** valstart,short** valend, short** tokenend){
	short* p = line, *ks, *ke;
	while ((next_key_unquote(p, &ks, &ke, valstart,valend,&p))){
		if (match(ks, ke, key)) {
			if (tokenend) *tokenend = p;
			return -1;
		}
	}
	return 0;
}


static int get_id(const handler_info* info, /*OUT*/ unsigned short*  buffer, int* length){
	return copy_string(info->handler_id, buffer, length);
}


static short* substring(const short* str_begin, const short* str_end){
	int len;
	short* buf;
	len = str_end - str_begin + 1;

	buf = (short*)MALLOC((len+1) * sizeof(*str_begin));
	if (!buf) return 0;
	memcpy(buf,str_begin,len * sizeof(*str_begin));
	buf[len] = 0;

	return buf;
}

static short* substring_unquote(const short* str_begin, const short* str_end){
	if ( (*str_begin == '\'' && *str_end == '\'') || (*str_begin == '\"' && *str_end == '\"') ){
		str_begin++;
		str_end--;
	}
	return substring(str_begin,str_end);
}


static short** substringarray(const short* str_begin, const short* str_end, int* pcount){
	short** result = 0,**p;
	const short *s,*s2;
	int len=1;
	if (str_begin == str_end) return 0;
	s = str_begin-1;
	while (++s<str_end) if (*s==XJAVA_ARRAY_DIVIDER) ++len;

	if (pcount) *pcount=len;
	
	result = (short**)MALLOC((sizeof(short**)) * (len+1));
	if (!result) return 0;

	s = str_begin;
	s2 = str_begin-1;	
	p = result;

	while (++s2<=str_end) {
	    if (*s2==XJAVA_ARRAY_DIVIDER) { 
			if (!(*p++ = (short*)substring_unquote(s,s2-1))){
				while (--p>=result) FREE(*p);
				FREE(result);
				return 0;
			}
			s=s2+1;
		}
	}

	if (s<str_end && !(*p++ = (short*)substring_unquote(s,str_end))){
		while (--p>=result) FREE(*p);
		FREE(result);
		return 0;
	}

	*p=0;
	return result;
}

static int get_integer(const short* str_begin, const short* str_end){
	int result = 0;
	int minus = 0;
	const short* buf = str_begin;
	if (!buf) return 0;
	if (*buf=='-') {minus=1;++buf;}
	while (*buf && buf <= str_end) {
		if(*buf>='0' && *buf<='9')  result = result * 10 + (*buf - '0');
		++buf;
	}
	return minus ? (-result) : result;
}


static handler_info* find_handler(const short* handler_name){
	int i=g_handler_infos_used;
	while (i){
		if (!javautil_str_wcscmp(g_handler_infos[--i]->handler_id, handler_name)) return g_handler_infos[i];
	}
	return 0;
}

static handler_info* find_handler_n(const short* handler_id,int len){
	int i=g_handler_infos_used;
	while (i){
		if (!javautil_str_wcsncmp(g_handler_infos[--i]->handler_id, handler_id, len)) return g_handler_infos[i];
	}
	return 0;
}


int read_access_list(){
    javautil_storage f;
    short* p, *ks, *ke, *vs, *ve;
    short* line;
    int length = MAX_BUFFER, res;
    handler_info* info;

	line = (short*)MALLOC(length*sizeof(*line));
	if (!line) return JAVACALL_CHAPI_ERROR_NO_MEMORY;

	res = open_db(ACCESSLIST_INDEX,&f,CHAPI_READ);
	if (res) {
		FREE(line);
		//no file to read
		return JAVACALL_OK;
	}

	while ((res=get_line(line,length,f))){

		if (res < 0) { //buffer too small
			length*=2;
			FREE(line);
			line = (short*)MALLOC(length*sizeof(*line));
			if (!line) break;
			continue;
		}

		p=line;
		if (!next_key_unquote(p,&ks,&ke,&vs,&ve,&p)) continue;
		if (info = (handler_info*)find_handler_n(ks,(int)(ke-ks)+1)){
			if (info->access_list) free_list((void**)info->access_list);
			info->access_list = (short**)substringarray(vs,ve,0);
		}
	}

	if (line) FREE(line);
	close_db(f);
	update_lastread(ACCESSLIST_INDEX);

	return 0;
}

static int read_action_map(){
    javautil_storage f;
    short* p, *ks, *ke;
    short* line;
    int length = MAX_BUFFER, res, count_localnames, count_locales, ia;
    handler_info* handler;
    action_info* action,*action2;

	line = (short*)MALLOC(length*sizeof(*line));
	if (!line) return JAVACALL_CHAPI_ERROR_NO_MEMORY;

	res = open_db(ACTIONMAP_INDEX,&f,CHAPI_READ);
	if (res) {
		FREE(line);
		//no file to read
		return JAVACALL_OK;
	}
	while ((res=get_line(line,length,f))){
		if (res < 0) { //buffer too small
			length*=2;
			FREE(line);
			line = (short*)MALLOC(length*sizeof(*line));
			if (!line) break;
			continue;
		}

		p=line;

		// handler
		if (!next_key_unquote(p,&ks,&ke,0,0,&p)) continue;
		if (!(handler = (handler_info*)find_handler_n(ks,(int)(ke-ks)+1))) continue;

		//action
		if (!next_key_unquote(p,&ks,&ke,0,0,&p)) continue;

		action = 0;
		for (ia = 0; ia<g_action_infos_used; ++ia){
			//find action with defined name for defined handler
			if (g_action_infos[ia]->handler == handler && (!javautil_str_wcsincmp(g_action_infos[ia]->actionname,ks,(int)(ke-ks)+1))) {
				action = g_action_infos[ia];
				if (action->locales && !(action->flag & TYPE_INFO_USE_REFERENCE)) {
					free_list((void**)action->locales);action->locales=0;
				    free_list((void**)action->localnames);action->localnames=0;
				}
				break;
			}
		}
		if (!action) continue;

		//locales
		if (!next_key(p,&ks,&ke,0,0,&p)) continue;
		action->locales = (short**)substringarray(ks,ke,&count_locales);

		//names
		if (!next_key(p,&ks,&ke,0,0,&p)) continue;
		action->localnames = (short**)substringarray(ks,ke,&count_localnames);

		//check
		if (!action->locales || !action->localnames || count_locales!=count_localnames){

			if (action->locales) free_list((void**)action->locales);
			action->locales = 0;

			if (action->localnames) free_list((void**)action->localnames);
			action->localnames = 0;
			continue;
		}

		//assign names for all actions with same name of this handler
		for (++ia; ia<g_action_infos_used; ++ia){
			action2 = g_action_infos[ia];
			if (action2->handler == handler && (!javautil_str_wcsicmp(action2->actionname,action->actionname))){
				if (action2->locales && !(action2->flag & TYPE_INFO_USE_REFERENCE)){ 
					free_list((void**)action->locales);
					free_list((void**)action->localnames);
				}
				action2->flag |= TYPE_INFO_USE_REFERENCE;
				action2->locales=action->locales;
				action2->localnames=action->localnames;
			}
		}
	}

	if (line) FREE(line);
	close_db(f);
	update_lastread(ACTIONMAP_INDEX);
	return 0;
}


int read_caps(){
	short* p, *ks, *ke, *vs, *ve;
	short *params=0, *handler_name=0, *type_name=0, *java_key;
	content_type_info* type = 0;
	action_info* action = 0;
	handler_info* handler = 0;
	javautil_storage f;
	int is_java;
	short* line;
	int length = MAX_BUFFER;
	int res = JAVACALL_CHAPI_ERROR_NO_MEMORY;
	
#ifdef DEBUG_OUTPUT
	wprintf(L"JAVACALL::read_registry\n");
#endif

	line = (short*)MALLOC(length*sizeof(*line));
	if (!line) return JAVACALL_CHAPI_ERROR_NO_MEMORY;

	res = open_db(CHAPI_INDEX,&f,CHAPI_READ);
	if (res) {
		FREE(line);
		//no file to read
		return JAVACALL_OK;
	}
	while ((res=get_line(line,length,f))){

		if (res < 0) { //buffer too small
			length*=2;
			FREE(line);
			line = (short*)MALLOC(length*sizeof(*line));
			if (!line) break;
			continue;
		}

		p = line;
		if (!next_key_unquote(p,&ks,&ke,0,0,&p)) continue;

		if (type_name) FREE(type_name);

		if (match(ks, ke,java_type)){
			type_name = (short*)MALLOC (sizeof(*type_name));
			*type_name = 0;
		} else {
			type_name = substring(ks,ke);
		}
		
		if (!type_name) break;


		type = (content_type_info*)new_content_type_info(type_name);
		if (!type) break;
		type_name = 0;

		is_java = find_key(line,L"x-java",&vs,&ve,&java_key);
		if (!is_java){
			//javautil_storage_read native content handler
		} else {
            int jflag = get_integer(vs, ve);
			if (!find_key(line,L"x-handlerid",&vs,&ve,0)) continue;
			handler_name = substring_unquote(vs,ve);

			if (handler = (handler_info*)find_handler(handler_name)){
				FREE (handler_name);
			} else {
				handler = (handler_info*)new_handler_info(handler_name);
				handler->flag |= TYPE_INFO_JAVA_HANDLER;
			}
			handler_name = 0;

            handler->jflag = jflag;

			p = line;
			action = 0;
						
			while ((next_key_unquote(p,&ks,&ke,&vs,&ve,&p))){
				if (match(ks, ke,L"x-action")) {
					action = (action_info*)new_action_info(type,handler);
					action->actionname = substring_unquote(vs,ve);
					action->flag |= TYPE_INFO_JAVA_HANDLER;
					continue;
				}
				if (match(ks, ke,L"x-suiteid") && !handler->suite_id) {handler->suite_id = substring_unquote(vs,ve);continue;}
				if (match(ks, ke,L"x-flag")) {handler->jflag = get_integer(vs,ve);continue;}
				if (match(ks, ke,L"x-classname") && !handler->classname) {handler->classname = substring_unquote(vs,ve);continue;}
				if (match(ks, ke,L"x-suffixes") && !type->suffixes) {type->suffixes = substringarray(vs,ve,0);continue;}
			}

			if (!action) {
				action = (action_info*)new_action_info(type, handler);
				action->actionname_const = DEFAULT_ACTION;
				action->flag |= TYPE_INFO_JAVA_HANDLER | TYPE_INFO_ACTION_DEFAULT;
			}

		}
		if (type_name) {FREE(type_name);type_name=0;}
		if (handler_name) {FREE(handler_name);handler_name=0;}
		if (params) {FREE(params);params=0;}
		handler = 0;
		action = 0;
		type = 0;
    }
    if (handler) pop_handler_info();
    if (line) FREE(line);
    close_db(f);
    update_lastread(CHAPI_INDEX);

    if (g_handler_infos_used){
	    read_access_list();
    }

    if (g_handler_infos_used){
	    read_action_map();
    }

    return 0;
}

int open_db(int db_index, javautil_storage* file, int flag){
	const char* db_path = 0;
	*file = 0;
	if (db_index == CHAPI_INDEX)
		db_path = chapi_fname;	
	else if (db_index == ACTIONMAP_INDEX)
		db_path = actionmap_fname;	
	else if (db_index == ACCESSLIST_INDEX)
		db_path = accesslist_fname;	
	else return JAVACALL_CHAPI_ERROR_BAD_PARAMS;


	if (flag == CHAPI_READ) {
		javautil_storage_open(db_path,JUS_O_RDONLY, file);
	} else 	if (flag == CHAPI_WRITE) {
		javautil_storage_open(db_path,JUS_O_RDWR | JUS_O_CREATE, file);
	} else if (flag == CHAPI_APPEND){
		javautil_storage_open(db_path, JUS_O_APPEND | JUS_O_CREATE, file);
	} else 
		return JAVACALL_CHAPI_ERROR_BAD_PARAMS;

	if (*file==JAVAUTIL_INVALID_STORAGE_HANDLE) {
		return JAVACALL_CHAPI_ERROR_IO_FAILURE;
	}

	return 0;
}


void close_db(javautil_storage file){
	if (file) javautil_storage_close(file);
}

/**********************************************************************************************************************/
/**
/**	Functions implemented by platform
/**
/**********************************************************************************************************************/

/**
 * Check native database was modified since time pointed in lastread
 *
 */
int is_modified(int index){
	unsigned long mtime=-1L;

	switch (index) {
		case CHAPI_INDEX: 
			javautil_storage_get_modified_time(chapi_fname,&mtime);
			return mtime>chapi_lastread;
		

		case ACTIONMAP_INDEX:
			javautil_storage_get_modified_time(actionmap_fname,&mtime);
			return mtime>actionmap_lastread;
		

		case ACCESSLIST_INDEX: 
			javautil_storage_get_modified_time(accesslist_fname,&mtime);
			return mtime>accesslist_lastread;

		default:
			return JAVACALL_CHAPI_ERROR_BAD_PARAMS;
	}
}

void update_lastread(int index){
	unsigned long mtime=0;

	switch (index) {
		case CHAPI_INDEX: 
			javautil_storage_get_modified_time(chapi_fname,&mtime);
			chapi_lastread = mtime;
			break;
		

		case ACTIONMAP_INDEX:
			javautil_storage_get_modified_time(actionmap_fname,&mtime);
			actionmap_lastread = mtime;
			break;
		

		case ACCESSLIST_INDEX: 
			javautil_storage_get_modified_time(accesslist_fname,&mtime);
			accesslist_lastread = mtime;
			break;
	}
}

void reset_lastread(){
	chapi_lastread = 0;
	actionmap_lastread = 0;
	accesslist_lastread = 0;
}


/**
 * Perform initialization of registry API
 *
 * @return JAVACALL_OK if initialization was successful, error code otherwise
 */
 javacall_result javacall_chapi_init_registry(void){
	char buf[MAX_BUFFER];
	int fpos=0;
	int res;

#ifdef DEBUG_OUTPUT
	wprintf(L"JAVACALL::javacall_chapi_init_registry()\n");
#endif
	
	res = read_caps();
	return res;
 }

/**
 * Finalize API, clean all used resources.
 *
 * @return nothing
 */
 void javacall_chapi_finalize_registry(void){
#ifdef DEBUG_OUTPUT
	wprintf(L"JAVACALL::javacall_chapi_finalize_registry()\n");
#endif

	clean_registry();
	reset_lastread();
 }

/**
 * Add new Content Handler to Registry
 *
 * @param content_handler_id unique ID of content handler
 *                           implemenation may not check this parameter on existence in registry
 *                           if handler id exists, function may be used to update unformation about handler
 *                           all needed uniqueness checks made by API callers if needed
 * @param content_handler_friendly_appname  the user-friendly application name of this content handler
 * @param suite_id identifier of the suite or bundle where content handler is located
 * @param class_name content handler class name
 * @param flag handler registration type
 * @param types handler supported content types array, can be null
 * @param nTypes length of content types array
 * @param suffixes handler supported suffixes array, can be null
 * @param nSuffixes length of suffixes array
 * @param actions handler supported actions array, can be null
 * @param nActions length of actions array
 * @param locales handler supported locales array, can be null
 * @param nLocales length of locales array
 * @param action_names action names for every supported action 
 *                                  and every supported locale
 *                                  should contain full list of actions for first locale than for second locale etc...
 * @param nActionNames length of action names array. This value must be equal 
 * to @link nActions multiplied by @link nLocales .
 * @param access_allowed_ids list of caller application ids (or prefixes to ids) that have allowed access to invoke this handler, can be null
 * @param nAccesses length of accesses list
 * @return JAVACALL_OK if operation was successful, error code otherwise
 */
javacall_result javacall_chapi_register_handler(
        javacall_const_utf16_string content_handler_id,
        javacall_const_utf16_string content_handler_friendly_appname,
        javacall_const_utf16_string suite_id,
        javacall_const_utf16_string class_name,
        javacall_chapi_handler_registration_type flag,
        javacall_const_utf16_string* content_types,     int nTypes,
        javacall_const_utf16_string* suffixes,  int nSuffixes,
        javacall_const_utf16_string* actions,   int nActions,  
        javacall_const_utf16_string* locales,   int nLocales,
        javacall_const_utf16_string* action_names, int nActionNames,
        javacall_const_utf16_string* access_allowed_ids,  int nAccesses){

    static javacall_utf16 invalidSuiteId[] = L"FFFFFFFF"; // INVALID_SUITE_ID string representation
	int result;
	javautil_storage file=0;
	int len;
	int itype, iact, il, iacc;
	char buf[MAX_LINE],*b;
	int idQuoted;

    if( suite_id == NULL ){ // native handler registration
        suite_id = invalidSuiteId;
    }

#ifdef DEBUG_OUTPUT
	wprintf(L"JAVACALL::javacall_chapi_register_handler(%s,%s,%s,0x%04x)\n",content_handler_id,suite_id,class_name, flag);
#endif

	result = open_db(CHAPI_INDEX,&file,CHAPI_APPEND);
	if (result != 0) return (javacall_result)result;

	//idQuoted = javautil_str_wcschr(content_handler_id,' ') || javautil_str_wcschr(content_handler_id,'\t') || javautil_str_wcschr(content_handler_id,'\'') || javautil_str_wcschr(content_handler_id,'=');
	idQuoted = 1;

	for (itype=0;itype<nTypes || (!nTypes && !itype);itype++){
		const unsigned short* type = nTypes?content_types[itype]:java_type;
		b=buf;

		// content type 
		b += append_string(b,type);
		*b++ = ';';
	
		// default action
		b += sprintf(b,"%s -suite \'",java_invoker);
		b += append_string(b,suite_id);
		b += sprintf(b,"\' -class \'");
		b += append_string(b,class_name);
		b += sprintf(b,"\' \'%%s\';");

		b += sprintf(b,"test=test -n \"${JVM_INVOKER}\";");

		if (content_handler_friendly_appname){
			b += sprintf(b,"description=");
			b += append_string(b,content_handler_friendly_appname);
			b += sprintf(b," Document;");
		}
		if (nSuffixes){
			b += sprintf(b,"nametemplate=%%s");
			b += append_string(b,nSuffixes>itype ? suffixes[itype] : suffixes[0]);
			*b++ = ';';
		}

		for (iact=0;iact<nActions;iact++){
			const unsigned short* action = actions[iact];
			//java action
			b += sprintf(b,"x-action='");
			b += append_string(b, action);
			b += sprintf(b,"\';");
		}
		
		b += sprintf(b,"x-java=%d;", flag);

		b += sprintf(b,"x-handlerid=");
		if (idQuoted) *b++ = '\'';
		b += append_string(b, content_handler_id);
		if (idQuoted) *b++ = '\'';
		*b++ = ';';
		
		b += sprintf(b,"x-suiteid=\'");
		b += append_string(b,suite_id);
		b += sprintf(b,"\';");

		b += sprintf(b,"x-classname='");
		b += append_string(b, class_name);
		b += sprintf(b,"\';");

		if (nSuffixes){
			int k;
			b += sprintf(b,"x-suffixes=");
			for (k=0;k<nSuffixes;++k) {
				b += append_string(b, suffixes[k]);
				if (k<nSuffixes-1) *b++ = XJAVA_ARRAY_DIVIDER;
			}
			*b++ = ';';
		}
		
		*b++ = '\n';

		len = b - buf;
		if (javautil_storage_write(file,buf,len)!=len) {
			result = JAVACALL_CHAPI_ERROR_IO_FAILURE;
			break;
		}
		result=0;
	}
	close_db(file);
	reset_lastread();

	if (result)  return (javacall_result)result;


    //update action local names
	if (nLocales){
		result = open_db(ACTIONMAP_INDEX,&file,CHAPI_APPEND);
		if (result != 0) return (javacall_result)result;

		javautil_storage_setpos(file,0,JUS_SEEK_END);

		for (iact=0;iact<nActions;iact++){
			const unsigned short* action = actions[iact];
			
			b=buf;
			//handler_id
			if (idQuoted) *b++ = '\'';
			b += append_string(b,content_handler_id);
			if (idQuoted) *b++ = '\'';
			*b++ = ';';

			//action
			*b++ = '\'';
			b += append_string(b,action);
			*b++ = '\'';
			*b++ = ';';

			//locales
			for (il=0;il<nLocales;++il){
				b += append_string(b,locales[il]);
				if (il<nLocales-1) *b++ = XJAVA_ARRAY_DIVIDER;
			}
			*b++ = ';';

			//actionnames
			for (il=0;il<nLocales;++il){
				*b++ = '\'';
				b += append_string(b,action_names[iact + nActions * il]);
				*b++ = '\'';
				if (il<nLocales-1) *b++ = XJAVA_ARRAY_DIVIDER;
			}

			*b++ = ';';
			*b++ = '\n';

			len=b-buf;
			if (javautil_storage_write(file,buf,len)!=len) {
				result = JAVACALL_CHAPI_ERROR_IO_FAILURE;
				break;
			}
			result = 0;
		}

		close_db(file);
		reset_lastread();

		if (result)  return (javacall_result)result;
	}

    //update access_list	
	if (nAccesses){
		result = open_db(ACCESSLIST_INDEX,&file,CHAPI_APPEND);
		if (result) return (javacall_result)result;


		javautil_storage_setpos(file,0,JUS_SEEK_END);

		while (1){
			b=buf;
			//handler_id
			if (idQuoted) *b++ = '\'';
			b += append_string(b,content_handler_id);
			if (idQuoted) *b++ = '\'';
			*b++ = '=';

		    for (iacc=0;iacc<nAccesses;++iacc){
				*b++ = '\'';
				b += append_string(b,access_allowed_ids[iacc]);
				*b++ = '\'';
				if (iacc<nAccesses-1) *b++ = XJAVA_ARRAY_DIVIDER;
			}

			*b++ = ';';
			*b++ = '\n';

			len=b-buf;
			if (javautil_storage_write(file,buf,len)!=len) {
				result = JAVACALL_CHAPI_ERROR_IO_FAILURE;
				break;
			}
			result = 0;
			break;
		}
		close_db(file);
		reset_lastread();
	}

	return (javacall_result)result;
}


/**
 * Enumerate all registered content handlers
 * Function should be called sequentially until JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS is returned
 * Returned between calls values are not guaranteed to be unique, it is up to caller to extract unique values if required
 *
 * @param pos_id  pointer to integer that keeps postion's information in enumeration
 *                before first call integer pointed by pos_id must be initialized to zero, 
 *                each next call it should have value returned in previous call
 *                pos_id value is arbitrary number or pointer to some allocated structure and not is index or position in enumeraion
 *                its value should not be interpreted by caller in any way
 *                Method javacall_chapi_enum_finish is called after last enum method call allowing implementation to clean allocated data
 *                If function returns error value pointed by pos_id MUST not be updated
 *				  
 * @param handler_id_out memory buffer receiving zero terminated string containing single handler id 
 * @param length pointer to integer initialized by caller to length of buffer,
 *               on return it set to length of data passing to buffer including the terminating zero
 *               if length of buffer is not enough to keep all data, length is set to minimum needed size and
 *               JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL is returned
 * @return JAVACALL_OK if operation was successful, 
 *         JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS if no more elements to return,
 *         JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL if buffer too small to keep result
 *         error code if failure occurs
 */
 javacall_result javacall_chapi_enum_handlers(int* pos_id, /*OUT*/ javacall_utf16*  handler_id_out, int* length){
      return (javacall_result)JAVACALL_NOT_IMPLEMENTED;
 }

/**
 * Enumerate registered content handlers that can handle files with given suffix
 * Search is case-insensitive
 * Function should be called sequentially until JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS is returned
 * Returned between calls values are not guaranteed to be unique, it is up to caller to extract unique values if required
 *
 * @suffix suffix of content data file with dot (for example: ".txt" or ".html") for which handlers are searched
 * @param pos_id  pointer to integer that keeps postion's information in enumeration
 *                before first call integer pointed by pos_id must be initialized to zero, 
 *                each next call it should have value returned in previous call
 *                pos_id value is arbitrary number or pointer to some allocated structure and not is index or position in enumeraion
 *                its value should not be interpreted by caller in any way
 *                Method javacall_chapi_enum_finish is called after last enum method call allowing implementation to clean allocated data
 *                If function returns error value pointed by pos_id MUST not be updated
 * @param handler_id_out memory buffer receiving zero terminated string containing single handler id 
 * @param length pointer to integer initialized by caller to length of buffer,
 *               on return it set to length of data passing to buffer including the terminating zero
 *               if length of buffer is not enough to keep all data, length is set to minimum needed size and
 *               JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL is returned
 * @return JAVACALL_OK if operation was successful, 
 *         JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS if no more elements to return,
 *         JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL if buffer too small to keep result
 *         error code if failure occurs
 */
javacall_result javacall_chapi_enum_handlers_by_suffix(javacall_const_utf16_string suffix, int* pos_id, /*OUT*/ javacall_utf16*  handler_id_out, int* length){
    return (javacall_result)JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Enumerate registered content handlers that can handle content with given content type
 * Search is case-insensitive
 * Function should be called sequentially until JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS is returned
 * Returned between calls values are not guaranteed to be unique, it is up to caller to extract unique values if required
 *
 * @content_type type of content data for which handlers are searched
 * @param pos_id  pointer to integer that keeps postion's information in enumeration
 *                before first call integer pointed by pos_id must be initialized to zero, 
 *                each next call it should have value returned in previous call
 *                pos_id value is arbitrary number or pointer to some allocated structure and not is index or position in enumeraion
 *                its value should not be interpreted by caller in any way
 *                Method javacall_chapi_enum_finish is called after last enum method call allowing implementation to clean allocated data
 *                If function returns error value pointed by pos_id MUST not be updated
 * @param handler_id_out memory buffer receiving zero terminated string containing single handler id 
 * @param length pointer to integer initialized by caller to length of buffer,
 *               on return it set to length of data passing to buffer including the terminating zero
 *               if length of buffer is not enough to keep all data, length is set to minimum needed size and
 *               JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL is returned
 * @return JAVACALL_OK if operation was successful, 
 *         JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS if no more elements to return,
 *         JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL if buffer too small to keep result
 *         error code if failure occurs
 */
 javacall_result javacall_chapi_enum_handlers_by_type(javacall_const_utf16_string content_type, int* pos_id, /*OUT*/ javacall_utf16*  handler_id_out, int* length){
 	int result;
	int index=*pos_id;

#ifdef DEBUG_OUTPUT
	wprintf(L"JAVACALL::javacall_chapi_enum_handlers_by_type(%s,%d)\n",content_type,*pos_id);
#endif
	if (!pos_id) return (javacall_result)JAVACALL_CHAPI_ERROR_BAD_PARAMS;

	if (!*pos_id){
		result = update_registry();
		if (result) return (javacall_result)result;
	}

	if (!g_handler_infos || !g_content_type_infos || !g_action_infos) return (javacall_result)JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS;
	while (index < g_action_infos_used){
		if (!javautil_str_wcsicmp(content_type,g_action_infos[index]->content_type->type_name)){
				result=get_id(g_action_infos[index]->handler,handler_id_out,length);
				if (!result){
					*pos_id = index+1;
				}
				return (javacall_result)result;
		}
		++index;
	}

	return (javacall_result)JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS;
 }

/**
 * Enumerate registered content handlers that can perform given action
 * Search is case-sensitive
 * Function should be called sequentially until JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS is returned
 * Returned between calls values are not guaranteed to be unique, it is up to caller to extract unique values if required
 *
 * @action action that handler can perform against content
 * @param pos_id  pointer to integer that keeps postion's information in enumeration
 *                before first call integer pointed by pos_id must be initialized to zero, 
 *                each next call it should have value returned in previous call
 *                pos_id value is arbitrary number or pointer to some allocated structure and not is index or position in enumeraion
 *                its value should not be interpreted by caller in any way
 *                Method javacall_chapi_enum_finish is called after last enum method call allowing implementation to clean allocated data
 *                If function returns error value pointed by pos_id MUST not be updated
 * @param handler_id_out memory buffer receiving zero terminated string containing single handler id 
 * @param length pointer to integer initialized by caller to length of buffer,
 *               on return it set to length of data passing to buffer including the terminating zero
 *               if length of buffer is not enough to keep all data, length is set to minimum needed size and
 *               JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL is returned
 * @return JAVACALL_OK if operation was successful, 
 *         JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS if no more elements to return,
 *         JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL if buffer too small to keep result
 *         error code if failure occurs
 */
 javacall_result javacall_chapi_enum_handlers_by_action(javacall_const_utf16_string action, int* pos_id, /*OUT*/ javacall_utf16*  handler_id_out, int* length){
      return (javacall_result)JAVACALL_NOT_IMPLEMENTED;
 }

/**
 * Enumerate registered content handlers located in suite (bundle) with given suite id
 * Search is case-sensitive
 * Function should be called sequentially until JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS is returned
 * Returned between calls values are not guaranteed to be unique, it is up to caller to extract unique values if required
 *
 * @suite_id suite id for which content handlers are searched
 * @param pos_id  pointer to integer that keeps postion's information in enumeration
 *                before first call integer pointed by pos_id must be initialized to zero, 
 *                each next call it should have value returned in previous call
 *                pos_id value is arbitrary number or pointer to some allocated structure and not is index or position in enumeraion
 *                its value should not be interpreted by caller in any way
 *                Method javacall_chapi_enum_finish is called after last enum method call allowing implementation to clean allocated data
 *                If function returns error value pointed by pos_id MUST not be updated
 * @param handler_id_out memory buffer receiving zero terminated string containing single handler id 
 * @param length pointer to integer initialized by caller to length of buffer,
 *               on return it set to length of data passing to buffer including the terminating zero
 *               if length of buffer is not enough to keep all data, length is set to minimum needed size and
 *               JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL is returned
 * @return JAVACALL_OK if operation was successful, 
 *         JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS if no more elements to return,
 *         JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL if buffer too small to keep result
 *         error code if failure occurs
 */
javacall_result javacall_chapi_enum_handlers_by_suite_id(
        javacall_const_utf16_string suite_id,
        int* pos_id, 
        /*OUT*/ javacall_utf16*  handler_id_out,
        int* length){
	int result;
	int index=*pos_id;

#ifdef DEBUG_OUTPUT
	wprintf(L"JAVACALL::javacall_chapi_enum_handlers_by_suite_id(%s,%d)\n",suite_id,*pos_id);
#endif

	if (!index){
		result = update_registry();
		if (result) return result;
	}

	while (index < g_handler_infos_used){
		if ((g_handler_infos[index]->flag & TYPE_INFO_JAVA_HANDLER) && !javautil_str_wcscmp(g_handler_infos[index]->suite_id,suite_id)){
			result=get_id(g_handler_infos[index],handler_id_out,length);
			if (!result){
				*pos_id = index+1;
			}
			return result;
		}
		index++;
	}

	return JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS;
}


/**
 * Enumerate registered content handler IDs that have the id parameter as a prefix
 * Search is case-sensitive
 * Function should be called sequentially until JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS is returned
 * Returned between calls values are not guaranteed to be unique, it is up to caller to extract unique values if required
 *
 * @param id      a string used for registered content handlers searching
 * @param pos_id  pointer to integer that keeps postion's information in enumeration
 *                before first call integer pointed by pos_id must be initialized to zero, 
 *                each next call it should have value returned in previous call
 *                pos_id value is arbitrary number or pointer to some allocated structure and not is index or position in enumeration
 *                its value should not be interpreted by caller in any way
 *                Method javacall_chapi_enum_finish is called after last enum method call allowing implementation to clean allocated data
 *                If function returns error value pointed by pos_id MUST not be updated
 * @param handler_id_out memory buffer receiving zero terminated string containing single handler id 
 * @param length pointer to integer initialized by caller to length of buffer,
 *               on return it set to length of data passing to buffer including the terminating zero
 *               if length of buffer is not enough to keep all data, length is set to minimum needed size and
 *               JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL is returned
 * @return JAVACALL_OK if operation was successful, 
 *         JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS if no more elements to return,
 *         JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL if buffer too small to keep result
 *         error code if failure occurs
 */
javacall_result javacall_chapi_enum_handlers_by_prefix(javacall_const_utf16_string id, 
    int* pos_id, /*OUT*/ javacall_utf16* handler_id_out, int* length)
{
	int result;
	int index=*pos_id;
	int id_len = javautil_str_wcslen( id );

#ifdef DEBUG_OUTPUT
	wprintf(L"JAVACALL::javacall_chapi_enum_handlers_by_prefix(%s,%d)\n",id,*pos_id);
#endif

	if (!index){
		result = update_registry();
		if (result) return result;
	}

	while (index < g_handler_infos_used){
		if ( javautil_str_wcslen( g_handler_infos[index]->handler_id ) > id_len &&
				0 == javautil_str_wcsncmp(id, g_handler_infos[index]->handler_id, id_len) ){
			result=get_id(g_handler_infos[index],handler_id_out,length);
			if (!result){
				*pos_id = index+1;
			}
			return result;
		}
		index++;
	}

	return JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS;
}

/**
 * Enumerate registered content handler IDs that are a prefix of the 'id' parameter.
 * Content handler ID equals to the 'id' parameter if exists must be included in returned sequence.
 * Search is case-sensitive
 * Function should be called sequentially until JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS is returned
 * Returned between calls values are not guaranteed to be unique, it is up to caller to extract unique values if required
 *
 * @param id      a string used for registered content handlers searching
 * @param pos_id  pointer to integer that keeps postion's information in enumeration
 *                before first call integer pointed by pos_id must be initialized to zero, 
 *                each next call it should have value returned in previous call
 *                pos_id value is arbitrary number or pointer to some allocated structure and not is index or position in enumeration
 *                its value should not be interpreted by caller in any way
 *                Method javacall_chapi_enum_finish is called after last enum method call allowing implementation to clean allocated data
 *                If function returns error value pointed by pos_id MUST not be updated
 * @param handler_id_out memory buffer receiving zero terminated string containing single handler id 
 * @param length pointer to integer initialized by caller to length of buffer,
 *               on return it set to length of data passing to buffer including the terminating zero
 *               if length of buffer is not enough to keep all data, length is set to minimum needed size and
 *               JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL is returned
 * @return JAVACALL_OK if operation was successful, 
 *         JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS if no more elements to return,
 *         JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL if buffer too small to keep result
 *         error code if failure occurs
 */
javacall_result javacall_chapi_enum_handlers_prefixes_of(javacall_const_utf16_string id, 
												int* pos_id, /*OUT*/ javacall_utf16* handler_id_out, int* length)
{
	int result;
	int index=*pos_id;
	int id_len = javautil_str_wcslen( id );

#ifdef DEBUG_OUTPUT
	wprintf(L"JAVACALL::javacall_chapi_enum_handlers_prefixes_of(%s,%d)\n",id,*pos_id);
#endif

	if (!index){
		result = update_registry();
		if (result) return result;
	}

	while (index < g_handler_infos_used){
		int handler_len = javautil_str_wcslen( g_handler_infos[index]->handler_id );
		if ( handler_len <= id_len &&
				0 == javautil_str_wcsncmp(id, g_handler_infos[index]->handler_id, handler_len) ){
			result=get_id(g_handler_infos[index],handler_id_out,length);
			if (!result){
				*pos_id = index+1;
			}
			return result;
		}
		index++;
	}

	return JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS;

}

/**
 * Enumerate all suffixes of content type data files that can be handled by given content handler or all suffixes 
 * registered in registry
 * Function should be called sequentially until JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS is returned
 * Returned between calls values are not guaranteed to be unique, it is up to caller to extract unique values if required
 *
 * @content_handler_id unique id of content handler for which content data files suffixes are requested
 *                     if  content_handler_id is null suffixes for all registered handlers are enumerated
 * @param pos_id  pointer to integer that keeps postion's information in enumeration
 *                before first call integer pointed by pos_id must be initialized to zero, 
 *                each next call it should have value returned in previous call
 *                pos_id value is arbitrary number or pointer to some allocated structure and not is index or position in enumeraion
 *                its value should not be interpreted by caller in any way
 *                Method javacall_chapi_enum_finish is called after last enum method call allowing implementation to clean allocated data
 *                If function returns error value pointed by pos_id MUST not be updated
 * @param suffix_out memory buffer receiving zero terminated string containing content data file suffix
 * @param length pointer to integer initialized by caller to length of buffer,
 *               on return it set to length of data passing to buffer including the terminating zero
 *               if length of buffer is not enough to keep all data, length is set to minimum needed size and
 *               JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL is returned
 * @return JAVACALL_OK if operation was successful, 
 *         JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS if no more elements to return,
 *         JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL if buffer too small to keep result
 *         error code if failure occurs
 */
javacall_result javacall_chapi_enum_suffixes(javacall_const_utf16_string content_handler_id, int* pos_id, /*OUT*/ javacall_utf16*  suffix_out, int* length)
{
     return (javacall_result)JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Enumerate all content data types that can be handled by given content handler or all content types
 * registered in registry
 * Function should be called sequentially until JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS is returned
 * Returned between calls values are not guaranteed to be unique, it is up to caller to extract unique values if required
 *
 * @content_handler_id unique id of content handler for which content data types are requested
 *                     if  content_handler_id is null all registered content types are enumerated
 * @param pos_id  pointer to integer that keeps postion's information in enumeration
 *                before first call integer pointed by pos_id must be initialized to zero, 
 *                each next call it should have value returned in previous call
 *                pos_id value is arbitrary number or pointer to some allocated structure and not is index or position in enumeraion
 *                its value should not be interpreted by caller in any way
 *                Method javacall_chapi_enum_finish is called after last enum method call allowing implementation to clean allocated data
 *                If function returns error value pointed by pos_id MUST not be updated
 * @param type_out memory buffer receiving zero terminated string containing single content type
 * @param length pointer to integer initialized by caller to length of buffer,
 *               on return it set to length of data passing to buffer including the terminating zero
 *               if length of buffer is not enough to keep all data, length is set to minimum needed size and
 *               JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL is returned
 * @return JAVACALL_OK if operation was successful, 
 *         JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS if no more elements to return,
 *         JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL if buffer too small to keep result
 *         error code if failure occurs
 */
javacall_result javacall_chapi_enum_types(javacall_const_utf16_string content_handler_id, /*OUT*/ int* pos_id, javacall_utf16*  type_out, int* length)
{
     return (javacall_result)JAVACALL_NOT_IMPLEMENTED;
}


/**
 * Enumerate all actions that can be performed by given content handler with any acceptable content
 * or all possible actions mentioned in registry for all registered content handlers and types
 * Function should be called sequentially until JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS is returned
 * Returned between calls values are not guaranteed to be unique, it is up to caller to extract unique values if required
 *
 * @content_handler_id unique id of content handler for which possible actions are requested
 *                     if  content_handler_id is null all registered actions are enumerated
 * @param pos_id  pointer to integer that keeps postion's information in enumeration
 *                before first call integer pointed by pos_id must be initialized to zero, 
 *                each next call it should have value returned in previous call
 *                pos_id value is arbitrary number or pointer to some allocated structure and not is index or position in enumeraion
 *                its value should not be interpreted by caller in any way
 *                Method javacall_chapi_enum_finish is called after last enum method call allowing implementation to clean allocated data
 *                If function returns error value pointed by pos_id MUST not be updated
 * @param action_out memory buffer receiving zero terminated string containing single action name
 * @param length pointer to integer initialized by caller to length of buffer,
 *               on return it set to length of data passing to buffer including the terminating zero
 *               if length of buffer is not enough to keep all data, length is set to minimum needed size and
 *               JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL is returned
 * @return JAVACALL_OK if operation was successful, 
 *         JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS if no more elements to return,
 *         JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL if buffer too small to keep result
 *         error code if failure occurs
 */
javacall_result javacall_chapi_enum_actions(javacall_const_utf16_string content_handler_id, /*OUT*/ int* pos_id, javacall_utf16*  action_out, int* length)
{
     return (javacall_result)JAVACALL_NOT_IMPLEMENTED;
}


/**
 * Enumerate all locales for witch localized names of actions that can be performed by given handler exist in registry.
 * Function should be called sequentially until JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS is returned
 * Returned between calls values are not guaranteed to be unique, it is up to caller to extract unique values if required
 *
 * @content_handler_id unique id of content handler for which possible locales are requested
 *                     in this method content handler id should not be null
 * @param pos_id  pointer to integer that keeps postion's information in enumeration
 *                before first call integer pointed by pos_id must be initialized to zero, 
 *                each next call it should have value returned in previous call
 *                pos_id value is arbitrary number or pointer to some allocated structure and not is index or position in enumeraion
 *                its value should not be interpreted by caller in any way
 *                Method javacall_chapi_enum_finish is called after last enum method call allowing implementation to clean allocated data
 *                If function returns error value pointed by pos_id MUST not be updated
 * @param locale_out memory buffer receiving zero terminated string containing single supported locale
 * @param length pointer to integer initialized by caller to length of buffer,
 *               on return it set to length of data passing to buffer including the terminating zero
 *               if length of buffer is not enough to keep all data, length is set to minimum needed size and
 *               JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL is returned
 * @return JAVACALL_OK if operation was successful, 
 *         JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS if no more elements to return,
 *         JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL if buffer too small to keep result
 *         error code if failure occurs
 */
javacall_result javacall_chapi_enum_action_locales(javacall_const_utf16_string content_handler_id, /*OUT*/ int* pos_id, javacall_utf16* locale_out, int* length)
{
     return (javacall_result)JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Get localized name of actions that can be performed by given handler
 *
 * @content_handler_id unique id of content handler for which localized action name is requested
 * @action standard (local neutral) name of action for which localized name is requested
 * @locale name of locale for which name of action is requested,
           locale consist of two small letters containing ISO-639 language code and two upper letters containg ISO-3166 country code devided by "-" possibly extended by variant
 * @param local_action_out memory buffer receiving zero terminated string containing local action name
 * @param length pointer to integer initialized by caller to length of buffer,
 *               on return it set to length of data passing to buffer including the terminating zero
 *               if length of buffer is not enough to keep all data, length is set to minimum needed size and
 *               JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL is returned
 * @return JAVACALL_OK if operation was successful, 
 *         JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL if buffer too small to keep result
 *         error code if failure occurs
 */
javacall_result javacall_chapi_get_local_action_name(javacall_const_utf16_string content_handler_id, javacall_const_utf16_string action, javacall_const_utf16_string locale, javacall_utf16*  local_action_out, int* length)
{
     return (javacall_result)JAVACALL_NOT_IMPLEMENTED;
}


/**
 * Enumerate all caller names that accessible to invoke given handler
 * Function should be called sequentially until JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS is returned
 * Returned between calls values are not guaranteed to be unique, it is up to caller to extract unique values if required
 *
 * @content_handler_id unique id of content handler for which callers list is requested
 *                     content handler id should not be null
 * @param pos_id  pointer to integer that keeps postion's information in enumeration
 *                before first call integer pointed by pos_id must be initialized to zero, 
 *                each next call it should have value returned in previous call
 *                pos_id value is arbitrary number or pointer to some allocated structure and not is index or position in enumeraion
 *                its value should not be interpreted by caller in any way
 *                Method javacall_chapi_enum_finish is called after last enum method call allowing implementation to clean allocated data
 *                If function returns error value pointed by pos_id MUST not be updated
 * @param access_allowed_out memory buffer receiving zero terminated string containing single access allowed caller id
 * @param length pointer to integer initialized by caller to length of buffer,
 *               on return it set to length of data passing to buffer including the terminating zero
 *               if length of buffer is not enough to keep all data, length is set to minimum needed size and
 *               JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL is returned
 * @return JAVACALL_OK if operation was successful, 
 *         JAVACALL_CHAPI_ERROR_NO_MORE_ELEMENTS if no more elements to return,
 *         JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL if buffer too small to keep result
 *         error code if failure occurs
 */
javacall_result javacall_chapi_enum_access_allowed_callers(javacall_const_utf16_string content_handler_id, int* pos_id, /*OUT*/ javacall_utf16*  access_allowed_out, int* length)
{
     return (javacall_result)JAVACALL_NOT_IMPLEMENTED;
}



/**
 * Get the user-friendly application name of given content handler
 *
 * @content_handler_id unique id of content handler for which application name is requested
 * @param handler_frienfly_appname_out memory buffer receiving zero terminated string containing user-friendly application name of handler
 * @param length pointer to integer initialized by caller to length of buffer,
 *               on return it set to length of data passing to buffer including the terminating zero
 *               if length of buffer is not enough to keep all data, length is set to minimum needed size and
 *               JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL is returned
 * @return JAVACALL_OK if operation was successful, 
 *         JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL if buffer too small to keep result
 *         error code if failure occurs
 */
javacall_result javacall_chapi_get_content_handler_friendly_appname(javacall_const_utf16_string content_handler_id, /*OUT*/ javacall_utf16*  handler_frienfly_appname_out, int* length)
{
     return (javacall_result)JAVACALL_NOT_IMPLEMENTED;
}


/**
 * Get the combined location information of given content handler
 * returns suite_id, classname, and integer flag describing handler application
 *
 * @content_handler_id unique id of content handler for which application name is requested
 *                     content handler id is case sensitive
 * @param suite_id_out buffer receiving suite_id of handler, can be null
 *                     for native platform handlers suite_id is zero
 *                     if suite_id_out is null suite_id is not retrieved 
 * @param classname_out buffer receiving zero terminated string containing classname of handler, can be null
 *                      for native platform handlers classname is full pathname to native application
 *                      if classname_out is null class name is not retrieved 
 * @param classname_len pointer to integer initialized by caller to length of classname buffer
 * @param flag_out pointer to integer receiving handler registration type, can be null
 *                 if flag_out is null registration flag is not retrieved 
 * @return JAVACALL_OK if operation was successful, 
 *         JAVACALL_CHAPI_ERROR_BUFFER_TOO_SMALL if output buffer lenght is too small to keep result
 *         error code if failure occurs
 */
javacall_result javacall_chapi_get_handler_info(javacall_const_utf16_string content_handler_id,
				   /*OUT*/
				   javacall_utf16*  suite_id_out, int* suite_id_len,
				   javacall_utf16*  classname_out, int* classname_len,
				   javacall_chapi_handler_registration_type *flag_out){
    handler_info* info;
    int i;
    int res;

#ifdef DEBUG_OUTPUT
    wprintf(L"JAVACALL::javacall_chapi_get_handler_info(%s)\n",content_handler_id);
#endif

    res = update_registry();
    if (res) return res;

	for (i=0;i<g_handler_infos_used;++i){
		if (!javautil_str_wcscmp(g_handler_infos[i]->handler_id,content_handler_id)){
			info = g_handler_infos[i];
			if (info->flag & TYPE_INFO_JAVA_HANDLER){
				if (suite_id_out) {
					res = copy_string(info->suite_id,suite_id_out,suite_id_len);
					if (res) return res;
				}
				if (flag_out) *flag_out = info->jflag;
				if (classname_out) {
					res = copy_string(info->classname,classname_out,classname_len);
				}
            } else {
			    if (suite_id_out) *suite_id_out = 0;
			    if (flag_out) *flag_out = 0;
			    if (classname_out) {
				    res = copy_string(info->appname,classname_out,classname_len);
			    }
            }
			return res;
		}
	}
	return JAVACALL_CHAPI_ERROR_NOT_FOUND;
}

static javacall_bool is_access_allowed( handler_info* info, javacall_const_utf16_string caller_id ) {
	if (info->flag & TYPE_INFO_JAVA_HANDLER){
	    short** s;
		if (!caller_id || !*caller_id) return JAVACALL_TRUE; //system caller
		if (!info->access_list || !*info->access_list) return JAVACALL_TRUE;
		s = info->access_list;
		while (*s) { 
			int len = javautil_str_wcslen(*s);
			if (!javautil_str_wcsncmp(*s,caller_id,len)) return JAVACALL_TRUE;
			++s;
		}
		return JAVACALL_FALSE;
	}
	return JAVACALL_TRUE;
}

/**
 * Check if given caller has allowed access to invoke content handler
 *
 * @content_handler_id unique id of content handler
 * @caller_id tested caller application name
 *            caller have access to invoke handler if its id is prefixed or equal to id from access_allowed list passed to register function
 *            if access list provided during @link javacall_chapi_register_handler call was empty, all callers assumed to be trusted
 * @return JAVACALL_TRUE if caller is trusted
 *         JAVACALL_FALSE if caller is not trusted
 */
javacall_bool javacall_chapi_is_access_allowed(javacall_const_utf16_string content_handler_id, javacall_const_utf16_string caller_id){
	int res;
	handler_info* info;
	
#ifdef DEBUG_OUTPUT
	wprintf(L"JAVACALL::javacall_chapi_is_access_allowed(%s)\n",content_handler_id);
#endif
	
	res = update_registry();
	if (res) return res;
	
	info = find_handler((const short*)content_handler_id);
	if (!info) return JAVACALL_FALSE;
	
	return is_access_allowed( info, caller_id );
}


/**
 * Check if given action is supported by given content handler
 * @content_handler_id unique id of content handler
 * @action tested action name (local neutral name)
 * @return JAVACALL_TRUE if caller is trusted
 *         JAVACALL_FALSE if caller is not trusted
 */
javacall_bool javacall_chapi_is_action_supported(javacall_const_utf16_string content_handler_id, javacall_const_utf16_string action)
{
     return (javacall_bool)JAVACALL_NOT_IMPLEMENTED;
}


/**
 * Remove all information about handler from registry
 *
 * @param content_handler_id unique ID of content handler
 * @return JAVACALL_OK if operation was successful, error code otherwise
 */
javacall_result javacall_chapi_unregister_handler(javacall_const_utf16_string content_handler_id)
{
     return (javacall_result)JAVACALL_NOT_IMPLEMENTED;
}


/**
 * Finish enumeration call. Clean enumeration position handle
 * This method is called after caller finished to enumerate by some parameter
 * Can be used by implementation to cleanup object referenced by pos_id if required
 *
 * @param pos_id position handle used by enumeration method call, if pos_id is zero it should be ignored
 * @return nothing
 */
 void javacall_chapi_enum_finish(int pos_id){}


 
/**
 * Asks NAMS to launch specified MIDlet. <BR>
 * It can be called by JVM when Java content handler should be launched
 * or when Java caller-MIDlet should be launched.
 * @param suite_id suite Id of the requested MIDlet
 * @param class_name class name of the requested MIDlet
 * @param should_exit if <code>JAVACALL_TRUE</code>, then calling MIDlet 
 *   should voluntarily exit to allow pending invocation to be handled.
 *   This should be used when there is no free isolate to run content handler (e.g. in SVM mode).
 * @return result of operation.
 */
javacall_result javacall_chapi_ams_launch_midlet(int suite_id,
        javacall_const_utf16_string class_name,
        /* OUT */ javacall_bool* should_exit)
{
     return (javacall_result)JAVACALL_NOT_IMPLEMENTED;
}


/**
 * Sends invocation request to platform handler. <BR>
 * This is <code>Registry.invoke()</code> substitute for Java->Platform call.
 * @param invoc_id requested invocation Id for further references
 * @param handler_id target platform handler Id
 * @param invocation filled out structure with invocation params
 * @param without_finish_notification if <code>JAVACALL_TRUE</code>, then 
 *       @link javanotify_chapi_platform_finish() will not be called after 
 *       the invocation's been processed.
 * @param should_exit if <code>JAVACALL_TRUE</code>, then calling MIDlet 
 *   should voluntarily exit to allow pending invocation to be handled.
 * @return result of operation.
 */
javacall_result javacall_chapi_platform_invoke(int invoc_id,
        const javacall_utf16_string handler_id, 
        javacall_chapi_invocation* invocation, 
        /* OUT */ javacall_bool* without_finish_notification, 
        /* OUT */ javacall_bool* should_exit)
{
     return (javacall_result)JAVACALL_NOT_IMPLEMENTED;
}


/*
 * Called by platform to notify java VM that invocation of native handler 
 * is finished. This is <code>ContentHandlerServer.finish()</code> substitute 
 * after platform handler completes invocation processing.
 * @param invoc_id processed invocation Id
 * @param url if not NULL, then changed invocation URL
 * @param argsLen if greater than 0, then number of changed args
 * @param args changed args if @link argsLen is greater than 0
 * @param dataLen if greater than 0, then length of changed data buffer
 * @param data the data
 * @param status result of the invocation processing. 
 */
void javanotify_chapi_platform_finish(int invoc_id, 
        javacall_utf16_string url,
        int argsLen, javacall_utf16_string* args,
        int dataLen, void* data, 
        javacall_chapi_invocation_status status)
{
}


/**
 * Receives invocation request from platform. <BR>
 * This is <code>Registry.invoke()</code> substitute for Platform->Java call.
 * @param handler_id target Java handler Id
 * @param invocation filled out structure with invocation params
 * @param invoc_id assigned by JVM invocation Id for further references
 */
void javanotify_chapi_java_invoke(
        const javacall_utf16_string handler_id, 
        javacall_chapi_invocation* invocation, /* OUT */ int invoc_id)
{
}


/*
 * Called by Java to notify platform that requested invocation processing
 * is completed by Java handler.
 * This is <code>ContentHandlerServer.finish()</code> substitute 
 * for Platform->Java call.
 * @param invoc_id processed invocation Id
 * @param url if not NULL, then changed invocation URL
 * @param argsLen if greater than 0, then number of changed args
 * @param args changed args if @link argsLen is greater than 0
 * @param dataLen if greater than 0, then length of changed data buffer
 * @param data the data
 * @param status result of the invocation processing. 
 * @param should_exit if <code>JAVACALL_TRUE</code>, then calling MIDlet 
 *   should voluntarily exit to allow pending invocation to be handled.
 * @return result of operation.
 */
javacall_result javacall_chapi_java_finish(int invoc_id, 
        javacall_const_utf16_string url,
        int argsLen, javacall_const_utf16_string* args,
        int dataLen, void* data, javacall_chapi_invocation_status status,
        /* OUT */ javacall_bool* should_exit)
{
     return (javacall_result)JAVACALL_NOT_IMPLEMENTED;
}


 
 


