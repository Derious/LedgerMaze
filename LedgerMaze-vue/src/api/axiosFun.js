import axios from 'axios';

// 登录请求方法
const loginreq = (method, url, params) => {
    console.log(params)
    return axios({
        method: method,
        url: url,
        headers: {
          'Content-Type': 'application/json; charset=utf-8'
        },
        data: params,
        traditional: true,
        /*transformRequest: [
            function(data) {
                let ret = ''
                for (let it in data) {
                    ret +=
                        encodeURIComponent(it) +
                        '=' +
                        encodeURIComponent(data[it]) +
                        '&'
                }
                return ret
            }
        ]*/
    }).then(res => res.data);
};
// 通用公用方法
const req = (method, url, params) => {
    console.log(url)

    return axios({
        method: method,
        url: url,
        headers: {
          'Content-Type': 'application/json; charset=utf-8',
            //token: localStorage.getItem('logintoken')
        },
        data:params,
        traditional: true,
        //transformRequest: [
        //    function(data) {
                //let ret = ''
                //console.log(data)
                /*for (let it in data) {
                    ret +=
                        encodeURIComponent(it) +
                        '=' +
                        encodeURIComponent(data[it]) +
                        '&'
                }*/
                //return ret
       //     }
        //]
    }).then(res => res.data);
   /*}).then(function(ret) {
    console.log(ret.data.data);
    console.log(ret.config.params);
}); */
};

export {
    loginreq,
    req
}
