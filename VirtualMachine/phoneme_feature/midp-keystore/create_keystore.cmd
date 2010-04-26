@echo off
echo Creating device key store

if exist main.ks del main.ks

keytool -import -alias rootCA -file C:\CactusJack\BlueTools\Security\certificate-authority\certificates\BlueWhaleSystems_CA\cacert.cer -keystore main.ks -storepass 3edcvgy76tfcxsw2
keytool -import -alias manufacturerCA -file C:\CactusJack\BlueTools\Security\certificate-authority\certificates\BlueWhaleSystems_CA\certs\intermediatecert.cer -keystore main.ks -storepass 3edcvgy76tfcxsw2
keytool -import -alias PCA3ss_v4 -file "C:\CactusJack\BlueTools\Security\Root Download Package\VeriSign Roots\PCA3ss_v4.509" -keystore main.ks -storepass 3edcvgy76tfcxsw2
keytool -import -alias EQUIFAX -file "C:\CactusJack\BlueTools\Security\Root Download Package\GeoTrust Roots\GeoTrust Primary Roots\Equifax_Secure_Certificate_Authority.txt" -keystore main.ks -storepass 3edcvgy76tfcxsw2
keytool -list -v -keystore main.ks -storepass 3edcvgy76tfcxsw2

