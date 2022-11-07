/**
* 左边菜单
*/ 
<template>
  <el-menu default-active="1" :collapse="collapsed" collapse-transition router :default-active="$route.path" unique-opened class="el-menu-vertical-demo" background-color="#334157" text-color="#fff" active-text-color="#E6A23C">
    <div class="logobox">
      <span>区块链钱包</span>
      <img class="logoimg" src="../assets/img/wallet1.png" alt="">
    </div>
      <el-menu-item-group>
        <el-menu-item v-for="chmenu in walletmenu" :index="'/'+chmenu.url" :key="chmenu.menuid">
          <i class="iconfont" :class="chmenu.icon"></i>
          <span class="menufont" >{{chmenu.menuname}}</span>
        </el-menu-item>
      </el-menu-item-group>

  </el-menu>
</template>
<script>

export default {
  name: 'leftnav',
  data() {
    return {
      collapsed: false,
      allmenu: [],
      walletmenu:[]
    }
  },
  // 创建完毕状态(里面是操作)
  created() {
    // 获取图形验证码
    this.walletmenu = [
      {
        menuid: 101,
        icon: 'icon-cat-skuQuery',
        menuname: '账户信息',
        hasThird: 'N',
        url: 'wallet/accountInfo',
        menus: null   
    },
      {
        menuid: 102,
        icon: 'icon-cs-manage',
        menuname: '支票交易',
        hasThird: 'N',
        url: 'wallet/creditInfo',
        menus: null   
    }
    ]

    //this.allmenu = res.data
    
    // 监听
    this.$root.Bus.$on('toggle', value => {
      this.collapsed = !value
    })
  }
}
</script>
<style>
.el-menu-vertical-demo:not(.el-menu--collapse) {
  width: 200px;
  min-height: 400px;
}
.el-menu-vertical-demo:not(.el-menu--collapse) {
  border: none;
  text-align: center;
}
.el-menu-item-group__title {
  padding: 0px;
}
.el-menu-bg {
  background-color: #1f2d3d !important;
}
.el-menu {
  border: none;
}
.logobox {
  height: 40px;
  line-height: 40px;
  color: #f7f3f3;
  font-size: 21px;
  text-align: center;
  padding: 20px 0px;
}
.logoimg {
  height: 32px;
}
.menufont {
  font-size: 16px;
}
</style>