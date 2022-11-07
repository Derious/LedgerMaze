// 导入组件
import Vue from 'vue';
import Router from 'vue-router';
// 登录
import login from '@/views/login';
//注册
import register from '@/views/account/register';
// 首页
import index from '@/views/index';
/**
 * 基础菜单
 */
//钱包管理
//账户信息
import accountInfo from '@/views/wallet/accountInfo';
//支票交易
import creditInfo from '@/views/wallet/creditInfo';

// 启用路由
Vue.use(Router);
//解决编程式路由往同一地址跳转时会报错的情况
const originalPush = Router.prototype.push;
const originalReplace = Router.prototype.replace;
//push
Router.prototype.push = function push(location, onResolve, onReject) {
  //if (onResolve || onReject)
  //  return originalPush.call(this, location, onResolve, onReject);
  return originalPush.call(this, location).catch(err => err);
};

// 导出路由 
export default new Router({
    routes: [{
        path: '/',
        name: '',
        component: login,
        hidden: true,
        meta: {
            requireAuth: false
        }
    }, {
        path: '/login',
        name: '登录',
        component: login,
        hidden: true,
        meta: {
            requireAuth: false
        }
    },{
        path: '/account/register',
        name: '注册',
        component: register,
        hidden: true,
        meta: {
            requireAuth: false
        }
    },{
        path: '/index',
        name: '首页',
        component: index,
        iconCls: 'el-icon-tickets',
        children: [{
            path: '/wallet/accountInfo',
            name: '个人账户信息',
            component: accountInfo,
            iconCls: 'el-icon-s-home',
            meta: {
                requireAuth: true
            }
        },{
            path: '/wallet/creditInfo',
            name: '支票信息',
            component: creditInfo,
            iconCls: 'el-icon-tickets',
            meta: {
                requireAuth: true
            }
        },{
            path: '/wallet/performanceInfo',
            name: '性能展示',
            component: creditInfo,
            iconCls: 'el-icon-tickets',
            meta: {
                requireAuth: true
            }
        }]
    }]
})

