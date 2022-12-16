# UESTC AutoLogin Docker image
This repo base on [uestc-network-manager](https://github.com/wmdscjhdpy/uestc-network-manager).
## Usage
Create a folder, and create file `user.conf`, edit your information to your self data.
```conf
username=yourname
password=yourpasswd
#check interval (second)
interval=30
#ac_id, the login area, can be find on your login page link.
ac_id=1
kEncVer=srun_bx1
#login ISP,can be @dx-uestc(laboratory) or @dx(dorm)
kDomain=@dx-uestc
web=http://aaa.uestc.edu.cn
```
then use this command to run a image:
```
docker run -d --name autologin --restart=always -v /Your/Folder:/etc/uestc_networkmanager/ wmdscjhdpy/uestc-autologin
```
If you want to test this image, you can manually logout the school network, then start the container and use `docker logs autologin` to see the autologing status.

## More Infomation about user.conf
### kDomain:
When you are in laboratory:set `kDomain=@dx-uestc`.
When you are in dorm and using chinatelecom:set `kDomain=@dx`
If you are in dorm and using and using chinamobile, you can check your domain when you manually login and the `kDomain` will display on the success website link. If you find the correct chinamobile `kDomain`, welcome to contact us.

### ac_id
This parameter is determined by the region supplying the network.
When you are in laboratory, it probably set to 1
When you are in dorm, it probably set to 3
you also can check your domain when you manually login and the `ac_id` will display on the success website link.