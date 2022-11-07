/**
 * 个人账户信息
 */
<template>
  <div>
    <div>
    <el-breadcrumb separator-class="el-icon-arrow-right">
      <el-breadcrumb-item :to="{ path: '/' }">区块链钱包</el-breadcrumb-item>
      <el-breadcrumb-item>账户信息</el-breadcrumb-item>
    </el-breadcrumb>
      <el-form  :inline="true">
      <el-row >
        <el-col :span="24">          
          <el-form-item >
          </el-form-item>
        </el-col>
      </el-row>
      </el-form>
      <el-descriptions class="margin-middle"  :column="2"  border :contentStyle="CS" :label-style="LS">
      <template slot="extra">
        
      </template>
      <el-descriptions-item>
        <template slot="label">
          <i class="el-icon-user-solid"></i>
          用户ID
        </template>
        {{this.id}}
      </el-descriptions-item>
      <el-descriptions-item>
        <template slot="label">
          <i class="el-icon-user"></i>
          用户序列号
        </template>
        {{this.sn}}
      </el-descriptions-item>
    </el-descriptions>
    <el-descriptions  class="margin-top"  :column="1"  border :contentStyle="CS" :label-style="LS">
      <el-descriptions-item>
          <template slot="label">
            <i class="el-icon-location-outline"></i>
            用户公钥
          </template>
          {{this.pk}}
        </el-descriptions-item>
    </el-descriptions>
    <el-descriptions  class="margin-top"  :column="2"  border :contentStyle="CS" :label-style="LS">
        <el-descriptions-item>
          <template slot="label">
            <i class="el-icon-coin"></i>
            明文账户余额
          </template>
          {{this.pt_balance}}
        </el-descriptions-item>
        <el-descriptions-item>
          <template slot="label">
            <i class="el-icon-coin"></i>
            密文账户余额
          </template>
          {{this.ct_balance}}
          &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
          <el-image size="small" :src='ctFlag?cipherIcon:plainIcon' @click="changeValue"  class="icon"></el-image>
        </el-descriptions-item>
    </el-descriptions>   
    <el-form label-width="150px" :inline="true" :model="inForm">
      <el-row >
        <el-col :span="8">          
          <el-form-item >
          </el-form-item>
        </el-col>
        <el-col :span="8" align="center">
        </el-col>
        <el-col :span="8">
        </el-col>
      </el-row>
      <el-row >
        <el-col :span="8">          
          <el-form-item >
            <el-input size="small" type="hidden"></el-input>
          </el-form-item>
        </el-col>
        <el-col :span="8" align="center">
          <el-button  icon="el-icon-folder-add" type="primary" :loading="loading" class="title" @click="handletranstopt('inForm')">转账到明文钱包</el-button>
          <el-button  icon="el-icon-folder-add" type="success"  :loading="loading" class="title" @click="handletranstoct('inForm')">转账到密文钱包</el-button>
        </el-col>
        <el-col :span="8" align="left">
          &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
          <el-button size="small" icon="el-icon-refresh" @click="loadTransaction()" type="info" plain>查看历史交易明细</el-button>
        </el-col>
      </el-row>
    </el-form>
    </div>
    <el-divider content-position="center"  style="width:80%" v-if="transTableVisible"><span style="color: #E6A23C">&nbsp;&nbsp;&nbsp;历&nbsp;史&nbsp;交&nbsp;易&nbsp;明&nbsp;细&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span></el-divider>
    <div>
    <!--交易明细列表-->
    <el-table size="medium" border :data="listData" highlight-current-row header-cell-class-name="table_header" v-if="transTableVisible" v-loading="loading"  element-loading-text="拼命加载中" style="width: 100%;">
      <el-table-column align="center" type="selection" width="60">
      </el-table-column>
      <el-table-column sortable prop="ct_sn" label="交易序列号" width="200">
      </el-table-column>
      <el-table-column sortable prop="ct_hash" label="交易哈希值" width="500" >
      </el-table-column>
      <el-table-column sortable prop="pk_sender" label="发送方公钥" width="600" >
      </el-table-column>
      <el-table-column sortable prop="type" label="交易类型" >
      </el-table-column>
    </el-table>
    <!-- 分页组件 -->
    <Pagination v-bind:child-msg="pageparm" @callFather="callFather" v-if="transTableVisible"></Pagination>
    </div>
    <!-- 转账到明文钱包 -->
    <el-dialog :title="transpttitle" :visible.sync="transptFormVisible" width="20%" @click="closeDialog('Redeem')" class="transferdialog" >
      <div slot="title" class="header-title">
        <img style="width:24px;height:24px;" src="../../assets/img/redeem.png" alt="" >
        <span>&nbsp;转账至明文钱包</span>
      </div>
      <el-form label-width="120px" :model="transptForm" :rules="ptrules" ref="transptForm" >
        <el-form-item label="转账金额:" prop="value">
          <el-input size="small" v-model="transptForm.value" auto-complete="off" placeholder="请输入转账金额">
            <i slot="prefix" style="display: flex;align-items: center;vertical-align:middle;margin: 15px auto 50px 5px; ">
                  <img
                    style="width:10px;height:10px;"
                    src="../../assets/img/money.png"
                    alt=""/>
          </i>
          </el-input>
        </el-form-item>
      </el-form>
      <div slot="footer" class="dialog-footer">
        <el-button size="small" @click="closeDialog('Redeem')" type="primary" plain>取消</el-button>
        <el-button size="small" type="primary" :loading="loading" class="title" @click="submitRedeem('transptForm')">确定</el-button>
      </div>
    </el-dialog>
    <!-- 转账到密文钱包 -->
    <el-dialog :title="transcttitle" :visible.sync="transctFormVisible" width="20%" @click="closeDialog('Mint')" class="mintdialog">
      <div slot="title" class="header-title">
        <img style="width:24px;height:24px;" src="../../assets/img/mint.png" alt="" >
        <span>&nbsp;转账至密文钱包</span>
      </div>
      <el-form label-width="120px" :model="transctForm" :rules="ctrules" ref="transctForm">
        <el-form-item label="转账金额:" prop="value">
          <el-input size="small" v-model="transctForm.value" auto-complete="off" placeholder="请输入转账金额">
            <i slot="prefix" style="display: flex;align-items: center;vertical-align:middle;margin: 15px auto 50px 5px; ">
                  <img
                    style="width:10px;height:10px;"
                    src="../../assets/img/money.png"
                    alt="" />
          </i>
        </el-input>
        </el-form-item>
      </el-form>
      <div slot="footer" class="dialog-footer">
        <el-button size="small" @click="closeDialog('Mint')" type="success" plain>取消</el-button>
        <el-button size="small" type="success" :loading="loading" class="title" @click="submitMint('transctForm')">确定</el-button>
      </div>
    </el-dialog>
  </div>
</template>

<script>
import { validateInteger } from '../../utils/validate.js'
import { redeem,mint,getAccountinfo, requestAllTrans } from '../../api/companyMG'
import Pagination from '../../components/Pagination'
var pageNum
export default {
  data() {
    return {
      nshow: true, //switch开启
      fshow: false, //switch关闭
      loading: false, //是显示加载
      transptFormVisible: false, //控制向明文钱包转账页面显示与隐藏
      transctFormVisible: false, //控制向密文钱包转账页面显示与隐藏
      transpttitle: '转账至明文钱包',
      transcttitle:'转账至密文钱包',
      transTableVisible: false,
      showInfo:false,
      inForm: {
        page: 1,
        limit: 10,
      },
      transptForm: {
        id:'',
        value: '',
      },
      transctForm: {
        id:'',
        value: '',
      },
      companyState:'',
      // rules表单验证
      ptrules: {
        value:[
            { required: true, trigger: 'blur',validator:validateInteger }
        ]
      },
      ctrules: {
        value:[
            { required: true, trigger: 'blur',validator:validateInteger }
        ]
      },
      listData: [], //交易明细数据
      // 分页参数
      pageparm: {
        page:1,
        currentPage: 1,
        pageSize: 10,
        total: 10
      },
      userid:'',
      ctFlag:true,//密码标示 true表示当前是密码形式
      plainIcon:require('../../assets/img/eye-24.png'),//展示图标
      cipherIcon:require('../../assets/img/closed-eye-24.png'),//隐藏图标
      id : '',
      sn : '',
      pk : '',
      pt_balance : '',
      ct_balance : '',
      ct_balance_value : '',  
      ct_balance_CT:'', 
      CS: {
      'text-align': 'left',  
      'min-width': '250px',  
      'font-size': '15px',
      'word-break': 'break-all'  
      },
      LS: {
        'text-align': 'left',
        'font-weight': '600',
        'font-size': '14px',
        'color':'rgb(26, 26, 26)',
        'height': '40px',
        'width': '150px',
        'word-break': 'keep-all'
      }
    }
  },
  // 注册组件
  components: {
    Pagination
  },
  /**
   * 数据发生改变
   */

  /**
   * 创建完毕
   */
  created() {
    this.userid = sessionStorage.getItem('user')
    this.getInfo(this.userid)
  },

  /**
   * 里面的方法只有被调用才会执行
   */
  methods: {
    //获取初始值
      getInfo:function(parm){
          let inparm = {id:''}
          inparm.id = parm
          getAccountinfo(JSON.stringify(inparm))
              .then(res => {
                this.loading = false
                if ("id" in res) {
                  this.id = res.id
                  this.sn = res.sn
                  this.pk = res.pk
                  this.pt_balance = res.pt_balance
                  this.ct_balance = res.ct_balance_CT
                  this.ct_balance_value = res.ct_balance_value
                  this.ct_balance_CT = res.ct_balance_CT

                  this.$message({
                    type: 'success',
                    message: '用户账户信息获取成功'
                  })
                } else if("flag" in res){
                  this.$message({
                    type: 'info',
                    message: res.flag
                  })
                }
                else {
                  this.$message({
                    type: 'info',
                    message: res.msg
                  })
                }
              })
              .catch(err => {
                this.loading = false
                this.$message.error('获取用户账户信息失败，请稍后再试！')
              })
    },
    submitRedeem:function(formName){
        this.$refs[formName].validate(valid => {
          if (valid) {
            this.transptForm.id = this.id
            let params = JSON.stringify(this.transptForm)
            redeem(params)
              .then(res => {
                  this.loading = false
                  if(res == 'redeem success'){
                      /*this.$message({
                        type: 'success',
                        message: '向明文钱包转账成功'
                      })*/
                      this.$alert('向明文钱包转账成功', '转账结果', {
                            confirmButtonText: '确定',
                            type: 'success',
                            callback: action => {
                            }
                      });
                      this.transptFormVisible = false
                      this.getInfo(this.userid)
                  }else{
                    //this.$message.error('向明文钱包转账失败')
                      this.$alert('向明文钱包转账失败', '转账结果', {
                            confirmButtonText: '确定',
                            type: 'error',
                            callback: action => {
                            }
                      });
                      return false
                  }
              })
              .catch(err => {
                //this.transptFormVisible = false
                this.loading = false
                this.$message.error('转账失败，请稍后再试！')
              })
          } else {
            return false
          }
        })
    },
    submitMint:function(formName){
        this.$refs[formName].validate(valid => {
          if (valid) {
            this.transctForm.id = this.id
            let params = JSON.stringify(this.transctForm)
            console.log(params)
            mint(params)
              .then(res => {
                  this.loading = false
                  if(res == 'mint success'){
                    this.transctFormVisible = false
                    this.$alert('向密文钱包转账成功', '转账结果', {
                            confirmButtonText: '确定',
                            type: 'success',
                            callback: action => {
                            }
                      });
                    this.getInfo(this.userid)
                  }else {
                      //this.$message.error('向密文钱包转账失败，请稍后再试！')
                      this.$alert('向密文钱包转账失败，请稍后再试！', '转账结果', {
                            confirmButtonText: '确定',
                            type: 'error',
                            callback: action => {
                            }
                      });
                }
              })
              .catch(err => {
                //this.transptFormVisible = false
                this.loading = false
                this.$message.error('转账失败，请稍后再试！')
              })
          } else {
            return false
          }
        })
    },
      //显示向明文钱包转账页面界面
    handletranstopt: function(param) {
      this.transptFormVisible = true
      this.transptForm.value = ''
    },
      //显示向密文钱包转账页面界面
    handletranstoct: function(param) {
      this.transctFormVisible = true
      this.transctForm.value = ''
    },

    // 关闭编辑、增加弹出框
    closeDialog:function(formname) {
      if(formname == 'Redeem'){
        this.transptFormVisible = false
      }else {
        this.transctFormVisible = false
      }
    },
    /*获取交易明细,暂时用不到*/
    // 获取交易明细列表
    getdata:function() {
      this.loading = true
      let inparm = {id:''}
      inparm.id = this.userid
      requestAllTrans(inparm)
         .then(res => {
           this.loading = false
            if (res == []) {
                this.$message({
                  type: 'info',
                  message: '无交易'
                })
              } else {
                // 分页赋值
                  console.log(res)
                  this.pageparm.currentPage = this.inForm.page
                  this.pageparm.pageSize = this.inForm.limit
                  this.pageparm.total = res.length
                  pageNum = Math.ceil(this.total / this.pageparm.pageSize) || 1
                  this.totalPage = Object.keys(res).map(key => {
                    return res[key]
                  })
                  for (let i = 0; i < pageNum; i++) {
                    this.tableData = this.totalPage.slice(this.pageparm.pageSize * (this.pageparm.currentPage - 1), this.pageparm.pageSize * this.pageparm.currentPage)
                  }
                  this.listData = this.tableData
                  this.$message({
                        type: 'success',
                        message: '交易明细查询成功'
                  })
             }
         })
         .catch(err => {
           this.loading = false
           this.$message.error(err)
           //this.$message.error('交易明细查询失败，请稍后再试！')
         })
    },

    // 分页插件事件
    callFather(parm) {
        this.inForm.page = parm.currentPage
        this.inForm.limit = parm.pageSize
        this.getdata()
    },
    loadTransaction:function(){
        this.transTableVisible = true
        this.getdata()
    },
    changeValue:function(){
      //密文标示取反
      this.ctFlag=!this.ctFlag;
      this.ct_balance = this.ctFlag?this.ct_balance_CT:this.ct_balance_value
    }
  }
}
</script>

<style>
  .el-breadcrumb__inner {
    font-size: 15px;
  }

  .transferdialog .el-dialog__header{
      text-align: left;
      background-color: #eaf0f6  ;
      color:rgb(26, 26, 26);
      font-weight: 600;
      border-bottom: 1px solid #e8eaec;
  }
  .transferdialog .el-dialog__body{
      text-align: center;
      font-size:15px;
  }

  .transferdialog .el-dialog__footer {
      text-align: center;
  }
  .mintdialog .el-dialog__header{
      text-align: left;
      background-color: #ecf1e9 ;
      color:rgbrgb(26, 26, 26);
      font-weight: 600;
      border-bottom: 1px solid #e8eaec;
  }
  .mintdialog .el-dialog__body{

      text-align: center;
      font-size:15px;
  }

  .mintdialog .el-dialog__footer {
      text-align: center;
  }
  .transferdialog {
    border-radius: 15px;
  }
  .icon{
    vertical-align:middle;
  }
  .table_header{
    background-color:rgba(200, 226, 242, 0) !important;
    color:#3f5e88 !important;
    font-size:14px!important;
    font-weight: 900 !important;
  }
  .header-title{
      display:flex ;
      align-items: center;
  }
  .el-descriptions__body {
  width: 56%;
  padding-left: 22%;
}
</style>


