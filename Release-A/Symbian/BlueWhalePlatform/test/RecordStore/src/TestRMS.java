import java.io.ByteArrayOutputStream;
import java.io.IOException;

import javax.microedition.rms.InvalidRecordIDException;
import javax.microedition.rms.RecordComparator;
import javax.microedition.rms.RecordEnumeration;
import javax.microedition.rms.RecordFilter;
import javax.microedition.rms.RecordStore;
import javax.microedition.rms.RecordStoreException;
import javax.microedition.rms.RecordStoreNotFoundException;
import javax.microedition.rms.RecordStoreNotOpenException;

import com.sun.midp.io.j2me.storage.RandomAccessStream;

public class TestRMS
{
    private int iTestCount;
    private int iPassCount;
    private int iFailCount;
    private byte[] iRec1 = {0x01,0x02,0x03,0x04};
    private byte[] iRec2 = {0x05,0x06,0x07,0x08,0x9};
    private byte[] iRec3 = {0x09,0x0a,0x0b,0x0c,0x0d,0x0e};
    private byte[][] iRecords = {iRec1,iRec2,iRec3};
    StringBuffer iWriteStream;
    
    private boolean iIntest;
    public TestRMS()
    {}

    protected void writeMessageln(String aMsg)
    {
        String msg = "<system-out>" + aMsg + "</system-out>\n";
        try
        {
            iWriteStream.append(msg);
        }
        catch(Exception e)
        {
            System.out.println("Exception while writing " +e);
        }
    }
    protected void writeXMLMessageln(String aMsg)
    {
        String msg = aMsg + "\n";
        try
        {
            iWriteStream.append(msg);
        }
        catch(Exception e)
        {
            System.out.println("Exception while writing " +e);
        }
    }
    protected void insertXMLMessageln(String aMsg)
    {
        String msg = aMsg + "\n";
        try
        {
            iWriteStream.insert(0,msg);
        }
        catch(Exception e)
        {
            System.out.println("Exception while writing " +e);
        }
    }
    
    protected void writeResults() throws IOException
    {
      RandomAccessStream fileStream = new RandomAccessStream();
      fileStream.connect("results.txt", RandomAccessStream.READ_WRITE_TRUNCATE);
      fileStream.writeBytes(iWriteStream.toString().getBytes(), 0, iWriteStream.toString().getBytes().length);
      fileStream.close();
    }
    
    protected void newTest(String aName) throws Exception
    {
        if(iIntest)
        {
            throw new Exception("Already in a test");
        }
        iTestCount++;
        writeXMLMessageln("<testcase name=\"" + aName + "\">");
        iIntest = true;
    }

    protected void passTest()
    {
        iPassCount++;
        writeXMLMessageln("</testcase>");
        iIntest = false;
    }
    protected void failTest()
    {
        iFailCount++;
        writeXMLMessageln("<failure>Test failed</failure>");
        writeXMLMessageln("</testcase>");
    }
    protected void assertTest(boolean aResult)
    {
        if(aResult)
        {
            passTest();
        }
        else
        {
            failTest();
        }   
    }

    protected boolean equals(byte[] aArray1, byte[] aArray2)
    {
        if(aArray1.length != aArray2.length)
        {
            return false;
        }
        for(int i=0;i<aArray1.length;i++)
        {
            if(aArray1[i] != aArray2[i])
            {
                return false;
            }
        }
        return true;            
    }
    
    protected void clearoutStore(String aStore)
    {
        try
        {
            RecordStore.deleteRecordStore(aStore);
        }
        catch(Exception e)
        {
         //ignore failure. just means the store didn't exists
        }
    }
    
    protected void openRecordStore() throws Exception
    {
        clearoutStore("Store1");
        try
        {
            newTest("Open and expect to fail");
            RecordStore store = RecordStore.openRecordStore("Store1",false);
        }
        catch(RecordStoreNotFoundException e)
        {
            writeMessageln("expected an exception " + e.toString());
            passTest();
        }

        try
        {
            newTest("Open and expect to fail");
            RecordStore store = RecordStore.openRecordStore("",false);
        }
        catch(IllegalArgumentException e)
        {
            writeMessageln("expected an exception " + e.toString());
            passTest();
        }
        try
        {
            newTest("Open and expect to fail");
            RecordStore store = RecordStore.openRecordStore("012345678901234567890123456789123",false);
        }
        catch(IllegalArgumentException e)
        {
            writeMessageln("expected an exception " + e.toString());
            passTest();
        }
        try
        {
            newTest("Open and expect to fail (not impletmented)");
            RecordStore store = RecordStore.openRecordStore("StoreX","BlueWhaleSystems","TestRMS");
        }
        catch(RecordStoreException e)
        {
            writeMessageln("expected an exception " + e.toString());
            passTest();
        }

        newTest("Open and expect to pass");
        RecordStore store = RecordStore.openRecordStore("Store1",true);
        passTest();
        store.closeRecordStore();
    }
    
    protected void deleteRecordStore() throws Exception
    {
        clearoutStore("deleteRecordStore#apath#another");
        clearoutStore("deleteRecordStore#apath");
        clearoutStore("deleteRecordStore");
        boolean pass = false;
        try
        {
            newTest("delete non existant record store");
            RecordStore.deleteRecordStore("deleteRecordStore");
        }
        catch(RecordStoreNotFoundException e)
        {
            writeMessageln("expected an exception " + e.toString());
            pass = true;
        }
        assertTest(pass);
        RecordStore store = RecordStore.openRecordStore("deleteRecordStore",true);
        int recordId = store.addRecord(iRec1,0,iRec1.length);
        recordId = store.addRecord(iRec1,0,iRec1.length);
        recordId = store.addRecord(iRec1,0,iRec1.length);
        recordId = store.addRecord(iRec1,0,iRec1.length);
        store.closeRecordStore();
        newTest("Delete non empty record store");
        RecordStore.deleteRecordStore("deleteRecordStore");
        try
        {
            store = RecordStore.openRecordStore("deleteRecordStore",false);
        }
        catch(RecordStoreNotFoundException e)
        {
            writeMessageln("expected an exception " + e.toString());
            passTest();
        }
        
        store = RecordStore.openRecordStore("deleteRecordStore#apath",true);
        recordId = store.addRecord(iRec1,0,iRec1.length);
        recordId = store.addRecord(iRec1,0,iRec1.length);
        recordId = store.addRecord(iRec1,0,iRec1.length);
        recordId = store.addRecord(iRec1,0,iRec1.length);
        store.closeRecordStore();
        newTest("delete store in a dir");
        RecordStore.deleteRecordStore("deleteRecordStore#apath");
        passTest();
        
        store = RecordStore.openRecordStore("deleteRecordStore#apath",true);
        recordId = store.addRecord(iRec1,0,iRec1.length);
        recordId = store.addRecord(iRec1,0,iRec1.length);
        recordId = store.addRecord(iRec1,0,iRec1.length);
        recordId = store.addRecord(iRec1,0,iRec1.length);
        store.closeRecordStore();

        store = RecordStore.openRecordStore("deleteRecordStore#apath#another",true);
        recordId = store.addRecord(iRec1,0,iRec1.length);
        recordId = store.addRecord(iRec1,0,iRec1.length);
        recordId = store.addRecord(iRec1,0,iRec1.length);
        recordId = store.addRecord(iRec1,0,iRec1.length);
        store.closeRecordStore();
        
        newTest("delete store in a dir that contains another store");
        RecordStore.deleteRecordStore("deleteRecordStore#apath");
        passTest();
        newTest("Open embedded store");
        store = RecordStore.openRecordStore("deleteRecordStore#apath#another",false);
        passTest();
        store.closeRecordStore();
        
    }
    
    protected void addRecord() throws Exception
    {
        clearoutStore("addRecord");
        RecordStore store = RecordStore.openRecordStore("addRecord",true);
        
        newTest("Add a new record");
        int recordId = store.addRecord(iRec1,0,iRec1.length);
        passTest();
        
        newTest("Enumerate records");
        RecordEnumeration records = store.enumerateRecords(null,null,false);
        passTest();
        int count = 0;
        while(records.hasNextElement())
        {
            newTest("Get record");
            byte[] data = records.nextRecord();
            passTest();
            newTest("Compare data");
            assertTest(equals(data,iRec1));
            count++;
        }
        newTest("record count should be 1");
        assertTest(count == 1);
        store.closeRecordStore();

        store = RecordStore.openRecordStore("addRecord",true);
        records = store.enumerateRecords(null,null,false);
        byte[] data = records.nextRecord();
        boolean pass = false;
        try
        {
            newTest("getnext record should fail with InvalidRecordIDException");
            data = records.nextRecord();
        }
        catch(InvalidRecordIDException e)
        {
            pass = true;
        }
        catch(Exception e)
        {
            writeMessageln("Unexpected exception " + e.toString());
        }
        finally
        {
            assertTest(pass);
        }
        records.reset();
        newTest("getnext record after reset");
        int id = records.nextRecordId();
        assertTest(id == 0);

        records.reset();
        id = records.previousRecordId();
        pass = false;
        try
        {
            newTest("previousRecordId should fail with InvalidRecordIDException");
            id = records.previousRecordId();
        }
        catch(InvalidRecordIDException e)
        {
            pass = true;
        }
        catch(Exception e)
        {
            writeMessageln("Unexpected exception " + e.toString());
        }
        finally
        {
            assertTest(pass);
        }
        
        records.reset();
        data = records.previousRecord();
        pass = false;
        try
        {
            newTest("previousRecord() should fail with InvalidRecordIDException");
            data = records.previousRecord();
        }
        catch(InvalidRecordIDException e)
        {
            pass = true;
        }
        catch(Exception e)
        {
            writeMessageln("Unexpected exception " + e.toString());
        }
        finally
        {
            assertTest(pass);
        }
        clearoutStore("multiple");
        store = RecordStore.openRecordStore("multiple",true);

        newTest("Adding multiple records");
        recordId = store.addRecord(iRec1,0,iRec1.length);
        recordId = store.addRecord(iRec2,0,iRec2.length);
        recordId = store.addRecord(iRec3,0,iRec3.length);
        passTest();
        
        newTest("Enumerate store with multiple entries. no filter. no comparator");
        records = store.enumerateRecords(null,null,false);
        passTest();
        count = 0;
        while(records.hasNextElement())
        {
            newTest("Get record num " + new Integer(count).toString());
            int recid = records.nextRecordId();
            writeMessageln("recid " + recid);
            data = store.getRecord(recid);
            passTest();
            newTest("Compare data");
            assertTest(equals(data,iRecords[recid]));
            count++;
        }
        newTest("record count");
        assertTest(count == 3);
        store.closeRecordStore();
    }   

    protected void closeRecordStore() throws Exception
    {
        clearoutStore("Store3");
        RecordStore store = RecordStore.openRecordStore("Store3",true);
        newTest("close store");
        store.closeRecordStore();
        passTest();
        try
        {
            newTest("Enumerate and expect exception");
            RecordEnumeration records = store.enumerateRecords(null,null,false);
        }
        catch(RecordStoreNotOpenException e)
        {
            passTest();
        }
        catch(Exception e)
        {
            failTest();
        }
        
        try
        {
            newTest("Close and expect exception");
            store.closeRecordStore();
        }
        catch(RecordStoreNotOpenException e)
        {
            passTest();
        }
        catch(Exception e)
        {
            failTest();
        }
    }
    
    class SimpleTestRecordFilter implements RecordFilter
    {
        public SimpleTestRecordFilter()
        {}
        public boolean  matches(byte[] candidate)
        {
            return true;
        }
    }
    class FirstByte5TestRecordFilter implements RecordFilter
    {
        public FirstByte5TestRecordFilter()
        {}
        public boolean  matches(byte[] candidate)
        {
            if(candidate[0] == 0x05)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    class ReverseRecordComparator implements RecordComparator
    {
        public ReverseRecordComparator()
        {}
        public int compare(byte[] rec1, byte[] rec2) 
        {
            if(rec1[0] < rec2[0])
            {
                return RecordComparator.FOLLOWS;
            }
            else if(rec1[0] > rec2[0])
            {
                return RecordComparator.PRECEDES;
            }
            else
            {
                return RecordComparator.EQUIVALENT ;
            }
        }
    }

    class ForwardRecordComparator implements RecordComparator
    {
        public ForwardRecordComparator()
        {}
        public int compare(byte[] rec1, byte[] rec2) 
        {
            if(rec2[0] < rec1[0])
            {
                return RecordComparator.FOLLOWS;
            }
            else if(rec2[0] > rec1[0])
            {
                return RecordComparator.PRECEDES;
            }
            else
            {
                return RecordComparator.EQUIVALENT ;
            }
        }
    }

    protected void enumerateRecords() throws Exception
    {
        clearoutStore("enumerateRecords");
        RecordStore store = RecordStore.openRecordStore("enumerateRecords",true);

        int recordId = store.addRecord(iRec1,0,iRec1.length);
        recordId = store.addRecord(iRec2,0,iRec2.length);
        recordId = store.addRecord(iRec3,0,iRec3.length);

        newTest("plain enumerate");
        RecordEnumeration records = store.enumerateRecords(null,null,false);
        int count = 0;
        while(records.hasNextElement())
        {
            byte[] data = records.nextRecord();
            count++;
        }
        writeMessageln("count is " + new Integer(count).toString() + " store count " + new Integer(store.getNumRecords()).toString());
        assertTest(count == store.getNumRecords());
        RecordFilter filter = (RecordFilter) new SimpleTestRecordFilter();
        newTest("simple filter enumerate");
        records = store.enumerateRecords(filter,null,false);
        passTest();
        newTest("simple filter count entries");
        assertTest(records.numRecords() == store.getNumRecords());

        filter = (RecordFilter) new FirstByte5TestRecordFilter();
        newTest("first byte is 5 filter enumerate");
        records = store.enumerateRecords(filter,null,false);
        passTest();
        newTest("first byte is 5 count entries");
        writeMessageln("Number of records is " + new Integer(records.numRecords()).toString());
        assertTest(records.numRecords() == 1);
        
        newTest("first byte is 5 filter enumerate with update");
        records = store.enumerateRecords(filter,null,true);
        passTest();
        newTest("first byte is 5 count entries");
        writeMessageln("Number of records is " + new Integer(records.numRecords()).toString());
        assertTest(records.numRecords() == 1);

        newTest("Add a new matching record");
        recordId = store.addRecord(iRec2,0,iRec2.length);
        assertTest(records.numRecords() == 2);
        store.closeRecordStore();

    }
    protected void enumerateComparatorRecords() throws Exception
    {
        clearoutStore("store4");
        RecordStore store = RecordStore.openRecordStore("Store4",true);

        int recordId = store.addRecord(iRec1,0,iRec1.length);
        recordId = store.addRecord(iRec2,0,iRec2.length);
        recordId = store.addRecord(iRec3,0,iRec3.length);
        
        RecordComparator comparator = (RecordComparator) new ReverseRecordComparator();
        newTest("Reverse comparator");
        RecordEnumeration records = store.enumerateRecords(null,comparator,false);
        passTest();
        newTest("check record count after sort");
        assertTest(records.numRecords() == store.getNumRecords());
        int count = 0;
        while(records.hasNextElement())
        {
            byte[] data = records.nextRecord();
            newTest("check record after sort");
            assertTest(equals(data,iRecords[iRecords.length - count - 1]));
            count++;
        }
        
        comparator = (RecordComparator) new ForwardRecordComparator();
        newTest("Forward comparator");
        records = store.enumerateRecords(null,comparator,false);
        passTest();
        newTest("check record count after sort");
        assertTest(records.numRecords() == store.getNumRecords());
        count = 0;
        while(records.hasNextElement())
        {
            byte[] data = records.nextRecord();
            newTest("check record after sort");
            assertTest(equals(data,iRecords[count]));
            count++;
        }

        store.closeRecordStore();
    }   
    
    protected void deleteFromSortedStore() throws Exception
    {
        clearoutStore("deleteFromSortedStore");
        RecordStore store = RecordStore.openRecordStore("deleteFromSortedStore",true);
        int recordId = store.addRecord(iRec1,0,iRec1.length);
        recordId = store.addRecord(iRec2,0,iRec2.length);
        recordId = store.addRecord(iRec3,0,iRec3.length);

        RecordComparator comparator = (RecordComparator) new ReverseRecordComparator();
        RecordEnumeration records = store.enumerateRecords(null,comparator,false);
        // should now have a reversed data set
        newTest("Check that list is sorted");
        int lastID = records.previousRecordId();
        writeMessageln("lastID " + new Integer(lastID).toString());
        assertTest(lastID == 0);
        newTest("Delete from sorted list");
        store.deleteRecord(lastID);
        writeMessageln("number of records after delete " + new Integer(store.getNumRecords()).toString());
        assertTest(store.getNumRecords() == 2);
        
        store.closeRecordStore();
    }
    
    protected void resetEnumeration() throws Exception
    {
        clearoutStore("resetEnumeration");
        RecordStore store = RecordStore.openRecordStore("resetEnumeration",true);
        int recordId = store.addRecord(iRec1,0,iRec1.length);
        recordId = store.addRecord(iRec2,0,iRec2.length);
        recordId = store.addRecord(iRec3,0,iRec3.length);

        RecordEnumeration records = store.enumerateRecords(null,null,false);
        int firstrecordId = records.nextRecordId();
        int id = records.nextRecordId();
        newTest("Make sure record id is correct");
        assertTest(id == 1);
        records.reset();
        newTest("Make sure record id is now back at the start");
        id = records.nextRecordId();
        assertTest(id == firstrecordId);
        store.closeRecordStore();
        
    }

    protected void newPathNaming() throws Exception
    {
        clearoutStore("apath#newPathNaming");
        clearoutStore("apath\\newPathNaming");
        newTest("creating a store using # as a seperator");
        RecordStore store = RecordStore.openRecordStore("apath#newPathNaming",true);
        passTest();
        store.closeRecordStore();
        newTest("should be able to open using \\ in path");
        store = RecordStore.openRecordStore("apath\\newPathNaming",false);
        passTest();
        store.closeRecordStore();
        newTest("creating a store using # as a seperator with \\ on the end");
        store = RecordStore.openRecordStore("apath#\\",true);
        passTest();
        newTest("Adding a record");
        int recordId = store.addRecord(iRec1,0,iRec1.length);
        passTest();
        store.closeRecordStore();
        
    }   

    protected void getRecordSize() throws Exception
    {
        clearoutStore("getRecordSize");
        RecordStore store = RecordStore.openRecordStore("getRecordSize",true);
        int recordId = store.addRecord(iRec1,0,iRec1.length);
        newTest("getRecordSize for single record");
        writeMessageln("new record ID " + new Integer(recordId).toString());
        int size = store.getRecordSize(recordId);
        assertTest(size == iRec1.length);
        store.closeRecordStore();
        
        boolean pass = false;
        try
        {
            newTest("getRecordSize for single record with store closed");
            size = store.getRecordSize(recordId);
        }
        catch(RecordStoreNotOpenException e)
        {
            writeMessageln("expected an exception " + e.toString());
            pass = true;
        }
        assertTest(pass);
        
        store = RecordStore.openRecordStore("getRecordSize",true);
        pass = false;
        try
        {
            newTest("getRecordSize for invalid record");
            size = store.getRecordSize(12345678);
        }
        catch(InvalidRecordIDException e)
        {
            writeMessageln("expected an exception " + e.toString());
            pass = true;
        }
        assertTest(pass);
        store.closeRecordStore();
    }

    protected void getNextRecordID() throws Exception
    {
        clearoutStore("getNextRecordID");
        RecordStore store = RecordStore.openRecordStore("getNextRecordID",true);
        int recordId = store.addRecord(iRec1,0,iRec1.length);
        newTest("getNextRecordID for single record");
        int ID = store.getNextRecordID();
        assertTest(ID == recordId + 1);
        store.closeRecordStore();
    }

    protected void getRecord() throws Exception
    {
        clearoutStore("getRecord");
        RecordStore store = RecordStore.openRecordStore("getRecord",true);
        int recordId = store.addRecord(iRec1,0,iRec1.length);
        newTest("getRecord from store");
        byte[] data = store.getRecord(recordId);
        assertTest(equals(iRec1,data));
        store.closeRecordStore();

        store = RecordStore.openRecordStore("getRecord",true);
        newTest("getRecord from store API version 2");
        int dataSize = store.getRecord(recordId,data,0);
        assertTest(equals(iRec1,data));
        store.closeRecordStore();

    }
    
    protected void setRecord() throws Exception
    {
        clearoutStore("setRecord");
        RecordStore store = RecordStore.openRecordStore("setRecord",true);
        store.closeRecordStore();
        boolean pass = false;
        try
        {
            newTest("setRecord while not open");
            store.setRecord(0,new byte[10],0,0);
        }
        catch(RecordStoreNotOpenException e)
        {
            writeMessageln("expected an exception " + e.toString());
            pass = true;
        }
        assertTest(pass);
        store = RecordStore.openRecordStore("setRecord",true);
        pass = false;
        try
        {
            newTest("setRecord with invalid record id");
            store.setRecord(0,new byte[10],0,0);
        }
        catch(InvalidRecordIDException e)
        {
            writeMessageln("expected an exception " + e.toString());
            pass = true;
        }
        assertTest(pass);
        int recordId = store.addRecord(iRec1,0,iRec1.length);
        byte[] newData = {0x09,0x08,0x07};
        store.setRecord(recordId,newData,0,3);
        byte[] data = store.getRecord(recordId);
        newTest("setRecord readback ok");
        assertTest(equals(newData,data));
        store.closeRecordStore();
        
    }
	
	protected void listRecords() throws Exception
	{
		newTest("listRecords");
            
		String[] stores = RecordStore.listRecordStores();
		for(int i=0;i<stores.length;i++)
		{
			writeMessageln("entry " + stores[i]);
		}
		assertTest(true);
	}

    public void runTests() throws Exception
    {
        iTestCount = 0;
        iPassCount = 0;
        iFailCount = 0;

        iWriteStream= new StringBuffer();
        
        try
        {
            openRecordStore();
            deleteRecordStore();
            addRecord();
            closeRecordStore();
            enumerateRecords();
            enumerateComparatorRecords();
            deleteFromSortedStore();
            resetEnumeration();
            newPathNaming();
            getRecordSize();
            getNextRecordID();
            getRecord();
            setRecord();
			listRecords();
        }
        catch(Throwable e)
        {
            e.printStackTrace();
        }
		try
		{
			insertXMLMessageln("<testsuite name=\"RMS tests\" failures=\"" + Integer.toString(iFailCount)+ "\" tests=\"" + Integer.toString(iTestCount) + "\">");
            writeXMLMessageln("</testsuite>");
            writeResults();
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
        
    }

}
