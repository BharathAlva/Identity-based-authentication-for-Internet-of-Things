
#!/bin/sh

dir=`pwd`
echo $dir
s="SERVER"
if [ $1 = 'SERVER' ]
then
echo "Server is running on 3001"
$dir/server 3001
elif [ $1 = 'CLIENT2' ]
then
echo "Server(client2) is running on 3001"
$dir/client2 3001 1
else
echo "Provide option of server"
fi
