<template>
  <div class="login-wrap">
    <div class="nameclass" >
      <span >账户模型隐私保护方案演示</span>
    </div>
    <el-form label-position="left" :model="resForm" :visible.sync="resFormVisible" ref="resForm" :rules="rules"  label-width="0px" class="demo-ruleForm login-container">
      <h3 class="title">用户注册</h3>
      <el-form-item prop="id">
        <el-input type="text" prefix-icon="el-icon-user-solid" v-model="resForm.id" auto-complete="off" placeholder="用户名"></el-input>
      </el-form-item>
     <el-form-item prop="pt_balance">
        <el-input type="text"  v-model="resForm.pt_balance" auto-complete="pt_balance" placeholder="初始余额">            
          <i slot="prefix" style="display: flex;align-items: center;vertical-align:middle;margin: 15px auto 50px 5px; ">
                  <img
                    style="width:10px;height:10px;"
                    src="../../assets/img/money.png"
                    alt=""
                  />
          </i></el-input>
      </el-form-item>
     <el-form-item prop="pass">
          <el-input type="password" prefix-icon="el-icon-lock" v-model="resForm.pass" auto-complete="off" placeholder="登录密码"></el-input>
          <span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;密码须设置为8-20位数字、字母和特殊字符的组合</span>
     </el-form-item>
     <el-form-item prop="pass2">
        <el-input type="password" prefix-icon="el-icon-lock" v-model="resForm.pass2" auto-complete="off" placeholder="确认密码"></el-input>
      </el-form-item>
      <el-form-item style="width:100%;">
        <el-button type="primary" style="width:100%;" @click="submitForm('resForm')" :loading="logining">注册</el-button>
      </el-form-item>
      <el-form-item>
        <el-link  style="float:right;" type="white" @click="login()">返回登录</el-link>
      </el-form-item>
    </el-form>
    <!-- 显示用户注册返回信息界面 -->
    <el-dialog :title="infotitle" :visible.sync="infoFormVisible" width="25%" @click='closeDialog()'>
      <el-form label-width="150px" ref="infoForm" :model="infoForm" :rules="rules" class="login-form-input">
        <el-form-item label="用户ID:" prop="id">
          <el-input size="small" v-model="infoForm.id"  readonly="true"></el-input>
        </el-form-item>
        <el-form-item label="用户序列号:" prop="sn">
          <el-input size="small" v-model="infoForm.sn"  readonly="true" ></el-input>
        </el-form-item>
        <el-form-item label="初始明文余额:" prop="pt_balance">
          <el-input size="small" v-model="infoForm.pt_balance"  class="login-form-input" readonly="true">
          </el-input>
        </el-form-item>
        <el-form-item label="用户公钥:" prop="pk">
          <span></span>
          <el-input size="small" v-model="infoForm.pk"  readonly="true" ></el-input>
        </el-form-item>
        <el-form-item label="初始密文余额:" prop="ct_balance_CT">
          <el-input size="small" v-model="infoForm.ct_balance_CT" readonly="true"></el-input>
        </el-form-item>
        <el-form-item label="初始密文实际余额:" prop="ct_balance_value">
          <el-input type="password" size="small" v-model="infoForm.ct_balance_value" readonly="true"></el-input>
        </el-form-item>
      </el-form>
      <div slot="footer" class="dialog-footer">
        <el-button size="small" @click='closeDialog()'>确定</el-button>
      </div>
    </el-dialog>
  </div>
</template>
<script type="text/ecmascript-6">
import { register } from '../../api/companyMG'
import { validatePsdReg,validateInteger,validateUserid } from '../../utils/validate'

export default {
  name: 'register',
  data() {
      var validatePass2 = (rule, value, callback) => {
        if (!value) {
          callback(new Error('请输入密码'))
        }
        if (value !== this.resForm.pass) {
          callback(new Error('两次输入密码不一致!'))
        } else {
          validatePsdReg(rule, value, callback)
        }
      };
    return {
      //定义loading默认为false
      logining: false,
      // 记住密码
       resFormVisible:false,
       infoFormVisible:false,//控制注册返回弹出框是否显示
       infotitle:'注册成功',
       resForm: {
        id: '',
        pt_balance:'',
        pass: '',
        pass2:'',
      },
      infoForm: {
        id: '',
        sn:'',
        pt_balance:'',
        pk:'',
        ct_balance_CT:'',
        ct_balance_value:''
      },
      //rules前端验证
      rules: {
        id: [{ required: true, trigger: 'blur',validator:validateUserid }],
        pt_balance: [{ required: true, trigger: 'blur',validator:validateInteger}],
        pass: [{ required: true, trigger: 'blur', validator:validatePsdReg}],
        pass2: [{ required: true, trigger: 'blur',validator:validatePass2 }],
      }
    }
  },
  components: {
  },

  // 创建完毕状态(里面是操作)
  created() {
    this.$message({
      message: '用户ID、初始余额和密码不能为空',
      type: 'success'
    })

  },
   mounted() {
   },
  // 里面的函数只有调用才会执行
  methods: {

    //获取info列表
    submitForm(formName) {
      this.$refs[formName].validate(valid => {
        if (valid) {
          this.logining = true
          let params = JSON.stringify(this.resForm)
          register(params).then(res => {
              this.logining = false
              if(res == 'create account success'){
                this.$message({
                  type: 'success',
                  message: '用户创建成功,自动跳转至登录页面'
                })
                this.resForm.id = ''
                this.resForm.pt_balance = ''
                this.resForm.pass = ''
                this.resForm.pass2 = ''
                          //如果请求成功就让他2秒跳转路由
                setTimeout(() => {
                    this.$router.push({ path: '/login' })
                }, 1000)
              }else if(res == 'create account fail'){
                this.$message.error('用户创建失败')
                return false
              }
          }).catch(err => {
            this.logining = false
            this.$message.error('用户创建失败，请稍后再试！')
         })
        } else {
          this.$message.error('请输入正确的注册信息！')
          return false
        }
      })
    },
    login(){
      this.$router.push({ path: '/login' })
    },
    closeDialog() {
      this.infoFormVisible = false
    },
  }
}
</script>

<style>
.login-wrap {
  box-sizing: border-box;
  width: 100%;
  height: 100%;
  padding-top: 5%;
  background-image: url(../../assets/img/bg.png);
  /* background-color: #112346; */
  background-repeat: no-repeat;
  background-position: center right;
  background-size: 100%;
}
.login-form-input .el-input__inner {
    border: 0 none;
}
.nameclass{
  text-align: center;
  color: #fefeff; 
  font-size:80px;
  height:20%;
}
.login-container {
  border-radius: 10px;
  margin: 0px auto;
  width: 350px;
  padding: 30px 35px 15px 35px;
  background: #fff;
  border: 1px solid #eaeaea;
  text-align: left;
  box-shadow: 0 0 20px 2px rgba(0, 0, 0, 0.1);
}
.title {
  margin: 0px auto 40px auto;
  text-align: center;
  color: #505458;
}
.remember {
  margin: 0px 0px 35px 0px;
}
.code-box {
  text-align: right;
}
.codeimg {
  height: 40px;
}

</style>
