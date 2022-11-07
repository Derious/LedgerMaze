import axios from 'axios';
import { loginreq, req } from './axiosFun';

// 登录接口
export const login = (params) => { return req("post", "/api/AccountLogin", params) };
//注册接口
export const register = (params) => { return req("post", "/api/CreateAccount", params) };
//查询用户账户信息
export const getAccountinfo = (params) => { return req("post", "/api/RequestAccts", params) };
//转账到明文钱包
export const redeem = (params) => { return req("post", "/api/Redeem", params) };
//转账到密文钱包
export const mint = (params) => { return req("post", "/api/Mint", params) };
//发送支票
export const sendCheque = (params) => { return req("post", "/api/SendCheque", params) };
//刷新支票
export const refreshCheque = (params) => { return req("post", "/api/RefreshCheque", params) };
//接收支票
export const depositCheque = (params) => { return req("post", "/api/DepositCheque", params) };
//请求刷新支票
export const requestTrans = (params) => { return req("post", "/api/RequestTrans", params) };
//获取交易明细
export const requestAllTrans = (params) => { return req("post", "/api/RequestAllTrans", params) };