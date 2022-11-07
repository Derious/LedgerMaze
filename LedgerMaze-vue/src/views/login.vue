<template>
  <div class="login-wrap">
    <div class="nameclass" >
      <span >账户模型隐私保护方案演示</span>
    </div>
    <el-form label-position="left" :model="ruleForm" :rules="rules" ref="ruleForm" label-width="0px" class="demo-ruleForm login-container">
      <h3 class="title">用户登录</h3>
      <el-form-item prop="id">
        <el-input type="text"  prefix-icon="el-icon-user-solid" v-model="ruleForm.id" auto-complete="off" placeholder="用户ID"></el-input>
      </el-form-item>
      <el-form-item prop="pass">
        <el-input type="password" prefix-icon="el-icon-lock" v-model="ruleForm.pass" auto-complete="off" placeholder="登录密码"></el-input>
      </el-form-item>
      <el-form-item>
      <el-row>
        <el-col :span="10">
          <!--<el-form-item prop="code">-->
            <el-input type="text" v-model="ruleForm.code" auto-complete="off" placeholder="图形验证码" @keyup.enter.native="submitForm('ruleForm')"></el-input>

          <!--</el-form-item>-->
        </el-col>
        <el-col :span="2">
        </el-col>
        <el-col :span="10">
          <span @click="refreshCode" style="cursor: pointer;">
             <SIdentify :identifyCode="identifyCode" @click="refreshCode()"></SIdentify>
        </span>
        </el-col>
      </el-row>
      </el-form-item>
      <el-checkbox class="remember" v-model="rememberpwd">记住密码</el-checkbox>
      <el-form-item style="width:100%;">
        <el-button type="primary" style="width:100%;" @click="submitForm('ruleForm')" :loading="logining">登录</el-button>
      </el-form-item>
      <el-form-item>
        <!--<el-link style="float:left;" type="white" @click="retrievePWD">忘记密码</el-link>  -->
        <el-link  style="float:right;" type="white" @click="regis">还没有账号？点击注册</el-link>
      </el-form-item>
    </el-form>
  </div>

</template>
<script type="text/ecmascript-6">
import { login } from '../api/companyMG'
import { setCookie, getCookie, delCookie } from '../utils/util'
import SIdentify from '../components/VerificationCode'
import { validatePsdReg,validateUserid } from '../utils/validate'
export default {
  name: 'login',
  data() {
    return {
      //定义loading默认为false
      logining: false,
      // 记住密码
      rememberpwd: false,
       identifyCodes: "1234567890",
       identifyCode: "",
       resFormVisible:false,//控制隐藏与显示注册页面
      ruleForm: {
        id: '',
        pass: '',
        code: '',
        randomStr: '',
        codeimg: ''
      },
      //rules前端验证
      rules: {
        id: [{ required: true, trigger: 'blur',validator:validateUserid }],
        pass: [{ required: true, trigger: 'blur' ,validator:validatePsdReg}],
        code: [{ required: true, message: '请输入验证码', trigger: 'blur' }]
      }
    }
  },
  components: {
    SIdentify: SIdentify,
  },

  // 创建完毕状态(里面是操作)
  created() {
    // 获取图形验证码
    this.getcode()
    // 获取存在本地的用户名密码
    this.getuserpwd()

  },
   mounted() {
       this.identifyCode = "";
       this.makeCode(this.identifyCodes, 4)
   },
  // 里面的函数只有调用才会执行
  methods: {
    // 获取用户名密码
    getuserpwd() {
      if (getCookie('user') != '' && getCookie('pwd') != '') {
        this.ruleForm.id = getCookie('user')
        this.ruleForm.pass = getCookie('pwd')
        this.rememberpwd = true
        console.log(this.ruleForm.id);
      }
    },
     randomNum(min, max) {
       return Math.floor(Math.random() * (max - min) + min);
     },
     refreshCode() {
       this.identifyCode = "";
       this.makeCode(this.identifyCodes, 4)
     },

     makeCode(o, l) {
       for (let i = 0; i < l; i++) {
         this.identifyCode += this.identifyCodes[
         this.randomNum(0, this.identifyCodes.length)
         ];
       }
       console.log(this.identifyCode);
     },
    getcode(){

    },
    //获取info列表
    submitForm(formName) {
      if(this.ruleForm.code != this.identifyCode){
        this.$message.error('验证码输入错误！')
        return false
      }
      this.$refs[formName].validate(valid => {
        if (valid) {
          this.logining = true

          // 测试通道，不为空直接登录
          /*setTimeout(() => {
            this.logining = false
            this.$store.commit('login', 'true')
            localStorage.setItem('user', this.ruleForm.id)
            localStorage.setItem('userdata', JSON.stringify(this.ruleForm))
            this.$router.push({ path: '/wallet/accountinfo' })
          }, 1000)*/

          // 正式代码
          let params = JSON.stringify(this.ruleForm)
          login(params).then(res => {
            console.log(res)
            if(res == 'login success'){
              if (this.rememberpwd) {
                //保存帐号到cookie，有效期7天
                setCookie('user', this.ruleForm.id, 7)
                //保存密码到cookie，有效期7天
                setCookie('pwd', this.ruleForm.pass, 7)
                } else {
                delCookie('user')
                delCookie('pwd')
              }
              //如果请求成功就让他2秒跳转路由
              setTimeout(() => {
                this.logining = false
                //缓存token
                //localStorage.setItem('logintoken', res.data.token)
                // 缓存用户个人信息
                sessionStorage.setItem('user', this.ruleForm.id)
                //this.$root.userID = this.ruleForm.id
                //this.$root.userStatus = '1'
                //console.log(`2222222=${this.ruleForm.id}`)
                this.$store.commit('login', 'true')
                console.log(`2222222=${this.$store.state.userID}`)
                this.$router.push({ path: '/wallet/accountInfo' })
              }, 1000)
            }else{
              this.$message.error('用户登录失败，请稍后再试！')
              this.logining = false
              return false
            }
          }).catch(err => {
            this.logining = false
            this.$message.error('用户登录失败，请稍后再试！')
         })
        } else {
          // 获取图形验证码
          this.getcode()
          this.$message.error('请输入用户名密码！')
          this.logining = false
          return false
        }
      })
    },
    //注册
    regis(){
      //this.resFormVisible = true
      this.$router.push({ path: '/account/register' })
    },
    //忘记密码
    /*retrievePWD(){
      this.$router.push({ path: '/account/retrievePwd' })
    },*/
  }
}
</script>

<style scoped>
.login-wrap {
  box-sizing: border-box;
  width: 100%;
  height: 100%;
  padding-top: 5%;
  background-image: url(../assets/img/bg.png);
  background-repeat: repeat-y;
  background-position: center center;
  background-size: 100%;
  background-attachment: scroll;
}
.login-container {
  border-radius: 10px;
  margin: 0px auto;
  width: 350px;
  padding: 30px 35px 15px 35px;
  background:rgba(36, 36, 85, .5);;
  border: 1px solid #eaeaea;
  text-align: left;
  box-shadow: 0 0 20px 2px rgba(0, 0, 0, 0.1);
  color: #d3d7f7;
}
.nameclass{
  text-align: center;
  color: #fefeff; 
  font-size:80px;
  height:20%;
}
.title {
  margin: 0px auto 40px auto;
  text-align: center;
  color: #d3d7f7;
}
.remember {
  margin: 0px 0px 35px 0px;
  color: #d3d7f7;
}
.code-box {
  text-align: right;
}
.codeimg {
  height: 40px;
}
</style>
