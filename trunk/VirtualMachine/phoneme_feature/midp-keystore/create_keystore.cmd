@echo off
echo Creating device key store

if exist main.ks del main.ks

keytool -import -alias rootCA -file C:\CactusJack\BlueTools\Security\certificate-authority\certificates\BlueWhaleSystems_CA\cacert.cer -keystore main.ks -storepass 3edcvgy76tfcxsw2
keytool -import -alias manufacturerCA -file C:\CactusJack\BlueTools\Security\certificate-authority\certificates\BlueWhaleSystems_CA\certs\intermediatecert.cer -keystore main.ks -storepass 3edcvgy76tfcxsw2
keytool -import -alias PCAG2V2 -file "C:\CactusJack\BlueTools\Security\Root Download Package\VeriSign Roots\Class3_PCA_G2_v2.509" -keystore main.ks -storepass 3edcvgy76tfcxsw2
keytool -list -v -keystore main.ks -storepass 3edcvgy76tfcxsw2

