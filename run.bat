cd GUI\bin\x86\Debug
start WpfApp1.exe localhost:8082 localhost:8080 /demo
start WpfApp1.exe localhost:8084 localhost:8080
cd ..\..\..\..\
cd Debug 
start ServerPrototype.exe localhost:8080
cd ..