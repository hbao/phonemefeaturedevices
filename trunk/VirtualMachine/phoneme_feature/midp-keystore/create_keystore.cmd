@echo off
echo Creating device key store

keytool -import -alias rootCA -file ..\certificates\BlueWhaleSystems_CA\cacert.cer -keystore main.ks -storepass 3edcvgy76tfcxsw2
keytool -import -alias manufacturerCA -file ..\certificates\BlueWhaleSystems_CA\certs\intermediatecert.cer -keystore main.ks -storepass 3edcvgy76tfcxsw2
keytool -list -v -keystore main.ks -storepass 3edcvgy76tfcxsw2