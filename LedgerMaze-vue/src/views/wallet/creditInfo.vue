/**
 * 支票交易
 */
<template>
  <div>
    <el-breadcrumb separator-class="el-icon-arrow-right">
      <el-breadcrumb-item :to="{ path: '/' }">区块链钱包</el-breadcrumb-item>
      <el-breadcrumb-item>支票交易</el-breadcrumb-item>
    </el-breadcrumb>
    <!-- 搜索筛选 -->
    <el-form :inline="true" :model="formInline" class="user-search">
      <el-form-item>
        <el-button size="medium" type="primary" icon="el-icon-tickets" @click="handlSend()">  发送支票</el-button>
        <el-button size="medium" type="success" icon="el-icon-refresh" @click="loadCredits()">请求刷新</el-button>
      </el-form-item>
    </el-form>
    <!--列表-->
    <el-table size="medium" :data="listData" highlight-current-row header-cell-class-name="table_header" v-loading="loading" border element-loading-text="拼命加载中" style="width: 100%;">
      <el-table-column align="center" type="selection" width="100">
      </el-table-column>
      <el-table-column sortable prop="ct_sn" label="交易序列号" width="150">
      </el-table-column>
      <el-table-column sortable prop="type" label="交易类型" width="150">
      </el-table-column>
      <el-table-column sortable prop="ct_hash" label="交易哈希值" width="500">
      </el-table-column>
      <el-table-column sortable prop="pk_sender" label="发送方公钥" width="500">
      </el-table-column>
      <el-table-column align="center" label="操作" min-width="150">
        <template slot-scope="scope">
            <el-button size="small" @click="handleCheck(scope.$index, scope.row)" type="primary" plain>刷新支票</el-button>
          <!--<el-button size="mini" type="danger" @click="deleteUser(scope.$index, scope.row)">删除</el-button>-->
        </template>
      </el-table-column>
    </el-table>
    <!-- 分页组件 -->
    <Pagination v-bind:child-msg="pageparm" @callFather="callFather"></Pagination>
    <!-- 发送支票页面 -->
    <el-dialog :title="sendtitle" :visible.sync="sendFormVisible" width="20%" @click="closeDialog('send')" class="senddialog">
      <div slot="title" class="header-title">
        <img src="../../assets/img/sendcredit1.png" alt="" >
        <span>&nbsp;发送支票</span>
      </div>
      <el-form label-width="100px" :model="sendForm" :rules="sendrules" ref="sendForm" >
        <el-form-item label="发送方ID:" prop="sender_id" >
          <el-input  v-model="sendForm.sender_id" auto-complete="off" readonly class="inputclass"></el-input>
        </el-form-item>
        <el-form-item label="接收方ID:" prop="receiver_id">
          <el-input  prefix-icon="el-icon-user-solid"  v-model="sendForm.receiver_id" auto-complete="off" placeholder="请输入接收方ID"></el-input>
        </el-form-item>
        <el-form-item label="支票金额:" prop="value">
          <el-input  v-model="sendForm.value" auto-complete="off" placeholder="请输入转账金额">          
            <i slot="prefix" style="display: flex;align-items: center;vertical-align:middle;margin: 15px auto 50px 5px; ">
                  <img
                    style="width:10px;height:10px;"
                    src="../../assets/img/money.png"
                    alt=""
                  />
          </i></el-input>
        </el-form-item>
      </el-form>
      <div slot="footer" class="dialog-footer">
        <el-button size="small" @click="closeDialog('send')" type="primary"  plain>取消</el-button>
        <el-button size="small" type="primary" :loading="loading" class="title" @click="submitSend('sendForm')">确定</el-button>
      </div>
    </el-dialog>
    <!-- 发送支票结果页面 -->
    <el-dialog :title="resulttitle" :visible.sync="resultFormVisible" width="30%" @click="closeDialog('result')">
      <el-form label-width="170px" :model="resultForm" ref="resultForm"  >
        <el-form-item label="用户ID:"  prop="id">
          <el-input size="small" v-model="resultForm.id" auto-complete="off" ></el-input>
        </el-form-item>
        <el-form-item label="用户公钥：" prop="pk">
          <el-input size="small" v-model="resultForm.pk" auto-complete="off"></el-input>
        </el-form-item>
        <el-form-item label="序列号：" prop="sn">
          <el-input size="small" v-model="resultForm.sn" auto-complete="off"></el-input>
        </el-form-item>
        <el-form-item label="明文钱包余额：" prop="pt_balance">
          <el-input size="small" v-model="resultForm.pt_balance" auto-complete="off"></el-input>
        </el-form-item>
        <el-form-item label="密文钱包余额：" prop="pt_balance">
          <el-input size="small" v-model="resultForm.ct_balance_CT" auto-complete="off"></el-input>
        </el-form-item>
        <el-form-item label="密文钱包余额（实际值）：" prop="pt_balance">
          <el-input size="small" v-model="resultForm.ct_balance_value" auto-complete="off"></el-input>
        </el-form-item>
      </el-form>
      <div slot="footer" class="dialog-footer">
        <el-button size="small" @click="closeDialog('result')">关闭</el-button>
      </div>
    </el-dialog>
    <!-- 支票刷新弹出页面 -->
    <el-dialog :title="checktitle" center :visible.sync="checkFormVisible" width="35%" @click="closeDialog('check')" class="creditdialog">
      <div slot="title" >
        <span class="creditclass">支票</span>
        <img src="../../assets/img/credit3.png" alt="" >
      </div>
      <el-form label-width="130px" :model="checkForm" ref="checkForm" >
        <el-form-item label="支票哈希值:"  prop="cheque_hash" class="labelclass">
          <el-input   type= "textarea" :autosize="{ minRows: 1, maxRows: 2}" v-model="checkForm.cheque_hash" auto-complete="off" ></el-input>
        </el-form-item>
        <el-form-item label="支票随机数:" prop="random" class="labelclass">
          <el-input  type= "textarea" :autosize="{ minRows: 1, maxRows: 2}" v-model="checkForm.random" auto-complete="off"></el-input>
        </el-form-item>
        <el-form-item label="支票金额:" prop="value" class="labelclass">
          <el-input  v-model="checkForm.value" auto-complete="off" class="inputclass"></el-input>
        </el-form-item>
      </el-form>
      <div slot="footer" class="dialog-footer">
        <el-button size="small" @click="closeDialog('check')" type="primary" plain>取消</el-button>
        <el-button size="small" type="primary" @click="handleReceive('checkForm')">接收支票</el-button>
      </div>
    </el-dialog>
    <!-- 分页组件 -->

  </div>
</template>

<script>
import Pagination from '../../components/Pagination'
import { sendCheque,refreshCheque,depositCheque,requestTrans } from '../../api/companyMG'
import { validateInteger,validateUserid } from '../../utils/validate'
var pageNum
export default {
  data() {
    return {
      nshow: true, //switch开启
      fshow: false, //switch关闭
      loading: false, //是显示加载
      creditFormVisible:false,
      checkFormVisible:false,//控制支票刷新页面显示与隐藏
      sendFormVisible:false,//控制支票发送页面显示与隐藏
      resultFormVisible:false,//控制发送支票明细页面显示与隐藏
      resulttitle:'支票明细',
      sendtitle:'发送支票',
      title:'',
      checktitle:'支票',
      sendForm:{
        sender_id:'',
        receiver_id:'',
        value:''
      },
      resultForm:{},
      checkForm: {},
      // rules表单验证
      sendrules: {
        value:[{ required: true,trigger: 'blur',validator:validateInteger }],
        receiver_id:[{ required: true, trigger: 'blur',validator:validateUserid }]
      },
      formInline: {
        page: 1,
        limit: 10,
        varLable: '',
        varName: '',
      },
      totalPage: [{
        ct_sn: '',
        pk_sender: '',
        ct_hash: '',
        type: ''
      }],
      tableData: [{
        ct_sn: '',
        pk_sender: '',
        ct_hash: '',
        type: ''
      }],
      listData: [], //用户数据
      // 分页参数
      pageparm: {
        currentPage: 1,
        pageSize: 10,
        total: 10
      },
      userid:''
    }
  },
  // 注册组件
  components: {
    Pagination
  },
  /**
   * 创建完毕
   */
  created() {
    this.userid = sessionStorage.getItem('user')
    this.getdata( )
  },
  /**
   * 里面的方法只有被调用才会执行
   */
  methods: {
    // 获取支票列表
    getdata:function() {
      this.loading = true
      let inparm = {id:''}
      inparm.id = this.userid
      requestTrans(inparm)
         .then(res => {
           this.loading = false
            if (res == []) {
                this.$message({
                  type: 'info',
                  message: '无待刷新的支票'
                })
              } else {
                // 分页赋值
                  this.pageparm.currentPage = this.formInline.page
                  this.pageparm.pageSize = this.formInline.limit
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
                        message: '支票刷新成功'
                  })
             }
         })
         .catch(err => {
           this.loading = false
           this.$message.error('支票刷新失败，请稍后再试！')
         })
    },

    // 分页插件事件
    callFather(parm) {
        this.formInline.page = parm.currentPage
        this.formInline.limit = parm.pageSize
        this.getdata()
    },
    loadCredits: function() {
        this.getdata()
    },
      //显示发送支票弹出框
    handlSend: function() {
        this.sendFormVisible = true
        this.sendForm.sender_id = this.userid
        this.sendForm.receiver_id = ''
        this.sendForm.value = ''
    },
      //显示支票信息
    handleResult: function(param) {
        this.resultFormVisible = true
        this.resultForm.id = param.id
        this.resultForm.pk = param.pk
        this.resultForm.sn = param.sn
        this.resultForm.pt_balance = param.pt_balance
        this.resultForm.ct_balance_CT = param.ct_balance_CT
        this.resultForm.ct_balance_value = param.ct_balance_value
    },
    //向后台发送支票
    submitSend: function(formName){
        this.$refs[formName].validate(valid => {
            if (valid) {
                let params = JSON.stringify(this.sendForm)
                sendCheque(params)
                .then(res => {
                    this.loading = false
                    if(res == 'send cheque success'){
                        /*this.$message({
                            type: 'success',
                            message: '支票发送成功'
                        })*/
                        this.$alert('支票发送成功!', '操作结果', {
                            confirmButtonText: '确定',
                            type: 'success',
                            callback: action => {
                            }
                        });                        
                        this.sendFormVisible = false
                    }else{
                        //this.$message.error('支票发送失败')
                        this.$alert('支票发送失败，请稍后再试!', '操作结果', {
                            confirmButtonText: '确定',
                            type: 'error',
                            callback: action => {
                            }
                        }); 
                    }
                })
                .catch(err => {
                    this.loading = false
                    this.$message.error('支票发送失败，请稍后再试！')
                })
          } else {
            return false
          }
        })
    },

    //支票刷新成功后弹出支票界面
    handleCheck: function(index, row) {
        let params = {id:'',hash:''}
        params.id = this.userid
        params.hash = row.ct_hash
        refreshCheque(JSON.stringify(params))
        .then(res => {
            if ("random" in res) {
                this.checkFormVisible = true
                this.checkForm.cheque_hash = res.cheque_hash
                this.checkForm.random = res.random
                this.checkForm.value = res.value
            } else {
                this.$message.error('支票刷新失败，请稍后再试！')
            }
        })
        .catch(err => {
            this.loading = false
            this.$message.error('支票刷新失败，请稍后再试！')
        })
    },
    //支票接收
    handleReceive: function(formName){
        this.$refs[formName].validate(valid => {
            if (valid) {
                let params = {id:'',cheque_hash:''}
                params.id = this.userid
                params.cheque_hash = this.checkForm.cheque_hash
                depositCheque( JSON.stringify(params))
                .then(res => {
                  this.loading = false
                  this.checkFormVisible = false
                  if (res == 'deposit cheque success') {
                    /*this.$message({
                    type: 'success',
                    message: '支票接收成功！'
                    })*/
                    this.$alert('支票接收成功！', '操作结果', {
                            confirmButtonText: '确定',
                            type: 'success',
                            callback: action => {
                            }
                    });
                    this.getdata()
                  } else {
                    //this.$message.error('支票验证失败！')
                    this.$alert('支票接收失败！', '操作结果', {
                            confirmButtonText: '确定',
                            type: 'error',
                            callback: action => {
                            }
                    });
                  }
                })
                .catch(err => {
                  this.loading = false
                  this.$message.error('支票接收失败，请稍后再试！')
                })
            } else {
                return false
            }
        })
    },
    // 关闭编辑、增加弹出框
    closeDialog(formName) {
        if(formName == 'send'){
            this.sendFormVisible = false
        }else if(formName == 'check'){
            this.checkFormVisible = false
        }else{
            this.resultFormVisible = false
        }
    }
  }
}
</script>

<style >
.user-search {
  margin-top: 20px;
}
.creditclass{
  font-size:22px;
}
.inputclass .el-input__inner  {
  border: 0 none;
  font-size:15px;
}
.senddialog .el-dialog__header{
      text-align: left;
      background-color: #eaf0f6 ;
      color:rgb(26, 26, 26);
      font-weight: 600;
      border-bottom: 1px solid #e8eaec;
  }
.senddialog .el-dialog__body{
      text-align: center;
      font-size:15px;
}
.senddialog .el-dialog__footer {
      text-align: center;
 }
.creditdialog .el-dialog__header{
      text-align: center;
      background-color: #eaf0f6 ;
      color:#061947;
      border-bottom: 1px solid #e8eaec;
  }
.creditdialog .el-dialog__body{
      text-align: center;
      font-size:15px;
  }

.creditdialog .el-dialog__footer {
      text-align: center;
      border-top: 1px solid #e8eaec;
  }
  .header-title{
      display:flex ;
      align-items: center;
  }
  .table_header{
    background-color:rgba(200, 226, 242, 0) !important;
    color:#3f5e88 !important;
    font-size:14px!important;
    font-weight: 900 !important;
  }
  .labelclass .el-form-item__label{
    font-size: 14px;
    font-weight: 600;
  }
</style>


