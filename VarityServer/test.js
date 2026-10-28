const nodemailer = require('nodemailer');
let transport = nodemailer.createTransport({
    host: 'smtp.qq.com',
    port: 465,
    secure: true,
    auth: {
        user: "3039018586@qq.com",
        pass: "hnvsdmozafxuddhg"
    }
});

async function test() {
    try{
        let res = await transport.sendMail({
            from: `"验证码服务" <3039018586@qq.com>`,
            to: "3039018586@qq.com",
            subject:"测试",
            text:"测试邮件"
        })
        console.log("成功",res.response)
    }catch(e){
        console.log("失败",e.message)
    }
}
test()
