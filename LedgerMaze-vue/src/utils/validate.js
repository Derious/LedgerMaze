export const validatePsdReg = (rule, value, callback) => {
    if (!value) {
      return callback(new Error('请输入密码'))
    }
    if (!/^(?![\d]+$)(?![a-zA-Z]+$)(?![^\da-zA-Z]+$)([^\u4e00-\u9fa5\s]){8,20}$/.test(value)) {
      callback(new Error('请输入8-20位英文字母、数字或者特殊字符且至少包含两种'))
    } else {
      callback()
    }
}

export function validateInteger(rule, value, callback) {
    if(!value) {
        return callback(new Error('请输入金额'))
    }
    setTimeout(() => {
        if(!Number(value)) {
            callback(new Error('请输入正确的金额'))
        } else {
            const re = /^[0-9]*[1-9][0-9]*$/
            const rsCheck = re.test(value)
            if(!rsCheck) {
                callback(new Error('请输入正确的金额'))
            } else {
                callback()
            }
        }
    }, 0);
}
export function validateUserid(rule, value, callback) {
    const reg =/^[\u0391-\uFFE5a-zA-Z0-9]+$/;
    if(value==''||value==undefined||value==null){
        callback(new Error('请输入正确的用户ID'));
    } else {
      if (!reg.test(value)){
          callback(new Error('请输入由中文、英文字母或数字组成的ID'));
      } else {
          callback();
      }
    }
 }