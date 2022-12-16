# UESTC 网络自动连接工具
[![example workflow name](https://github.com/ehds/uestc_network_manager/workflows/C/C++%20CI/badge.svg)](https://github.com/ehds/uestc_network_manager/actions)

# Linux(Ubuntu)操作系统
## 依赖
```
sudo apt-get install libssl-dev cmake git build-essential
```

## 克隆仓库
```
git clone https://github.com/wmdscjhdpy/uestc-network-manager.git
cd uestc-network-manager
git submodule update --init --recursive
```

## 编译
```
mkdir build
cd build && cmake ..
make
make install
```

## 用法
在 `/etc/uestc_networkmanager/user.conf` 路径下编辑参数

### kDomain:
教研室自动登录请使用 `kDomain=@dx-uestc`.
宿舍电信网登录请使用 `kDomain=@dx`
如果使用的是宿舍费电信网，可以自行查看登录页面链接中 `kDomain` 参数的值. 如果你有上文未提及的 `kDomain`对应运营商，欢迎提交issue。

### ac_id
This parameter is determined by the region supplying the network.
该值用来决定提供网络的区域。如果你是在教研室，应该设置为1
如果你是来自宿舍，应该设置为3（不确定，仅开发人员自测）
该参数同样可以通过登录页面的链接中确定`ac_id`后面的值来判断

### web
登录页面，默认的aaa.uestc.edu会自动指向可用的登录页面。

## 启动服务
```
systemctl enable uestcnetwork
systemctl start uestcnetwork
```

**Windows 和 OpenWrt 正在处理**