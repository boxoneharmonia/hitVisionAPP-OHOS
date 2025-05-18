// import serialPort from "libserialport.so";
// import util from '@ohos.util';
// import prompt from '@ohos.prompt';
// import Logger from '../model/Logger';
// import worker from '@ohos.worker'
//
//
// const workerInstance = new worker.Worker('workers/worker.ts', { name: "first worker in Stage model" });
// const TAG = '[IdoUart]'
// let uri = 'dataability:///com.ohos.settingsdata.DataAbility'
//
// let uartFd: number = -1;
// let oTimer = null;
//
//
// function anyToString(value: any): string {
//   if (typeof value === 'string') {
//     Logger.error(TAG, '已经是字符串，直接返回');
//     return value; // 如果已经是字符串，直接返回
//   } else if (typeof value === 'number' || typeof value === 'boolean') {
//     Logger.error(TAG, ' 数字或布尔值转换为字符串');
//     return value.toString(); // 数字或布尔值转换为字符串
//   } else if (value instanceof Uint8Array) {
//     Logger.error(TAG, ' Uint8Array 转换为字符串');
//     return bytesToString(value); // Uint8Array 转换为字符串
//   } else if (typeof value === 'object') {
//     Logger.error(TAG, '对象类型转换为 JSON 字符串');
//     return JSON.stringify(value); // 对象类型转换为 JSON 字符串
//   } else {
//     Logger.error(TAG, '其他类型，使用默认的 String() 转换');
//     return String(value); // 其他类型，使用默认的 String() 转换
//   }
// }
//
// // 将 Uint8Array 转换为字符串
// function uint8ArrayToString(byteArray: Uint8Array): string {
//   // 遍历每个字节，将其转换为字符并拼接成字符串
//   return Array.from(byteArray).map(byte => String.fromCharCode(byte)).join('');
// }
//
// // 将16进制字符串转换为普通字符串
// function hexStringToString(hexStr: string): string {
//   // 去掉字符串中的空格
//   const cleanHexStr = hexStr.replace(/\s+/g, '');
//
//   // 每两个字符表示一个字节，将其转换为字符
//   let str = '';
//   for (let i = 0; i < cleanHexStr.length; i += 2) {
//     const hexByte = cleanHexStr.substring(i, i + 2);
//     const charCode = parseInt(hexByte, 16); // 将16进制字节转换为数字
//     str += String.fromCharCode(charCode); // 将数字转换为字符并拼接
//   }
//   return str;
// }
//
// function bytesToString(bytes: Uint8Array): string {
//   let result = '';
//   for (let i = 0; i < bytes.length; i++) {
//     result += String.fromCharCode(bytes[i]);
//   }
//   return result;
// }
//
// function bytesToHexString(bytes: any): string {
//   // 将每个字节转换为两位的16进制数，并在每个字节之间加空格
//   // 如果数据已经是Uint8Array，直接返回
//   if (bytes instanceof Uint8Array) {
//     return Array.from(bytes)
//       .map((byte) => byte.toString(16).padStart(2, '0'))// 转换为16进制并确保是两位
//       .join(' '); // 用空格分隔每个字节的16进制表示
//   }
// }
//
// function hexStringToUint8Array(hexStr: String): any {
//   const len = hexStr.length / 2;
//   const bytes = new Uint8Array(len);
//   for (let i = 0; i < len; i++) {
//     bytes[i] = parseInt(hexStr.substr(i * 2, 2), 16);
//   }
//   return bytes;
// }
//
//
// // 将16进制字符串（例如 "e714 dc63 88"）转换为字节数组
// function hexStringToBytes(hexStr: string): Uint8Array {
//   // 去掉字符串中的空格
//   const cleanHexStr = hexStr.replace(/\s+/g, '');
//
//   // 将每两个字符转换为一个字节
//   const byteArray = new Uint8Array(cleanHexStr.length / 2);
//   for (let i = 0; i < cleanHexStr.length; i += 2) {
//     byteArray[i / 2] = parseInt(cleanHexStr.substring(i, i + 2), 16);
//   }
//   return byteArray;
// }
//
// // 将字节数组转换为16进制字符串
// function bytesToHex(byteArray: Uint8Array): string {
//   return Array.from(byteArray)
//     .map(byte => byte.toString(16).padStart(2, '0').toUpperCase())
//     .join(' ');
// }
//
// function hexStringToByteArray(hexString: string): Uint8Array {
//   hexString = hexString.replace(/\s/g, '');
//   const byteArray = new Uint8Array(hexString.length / 2);
//   for (let i = 0; i < hexString.length; i += 2) {
//     byteArray[i / 2] = parseInt(hexString.substring(i, i + 2), 16);
//   }
//   return byteArray;
// }
//
// function byteArrayToHexString(byteArray: Uint8Array): string {
//   let hexString = '';
//   for (let i = 0; i < byteArray.length; i++) {
//     let hex = byteArray[i].toString(16).padStart(2, '0');
//     hexString += hex;
//   }
//   return hexString;
// }
//
// function stringToUint8Array(str: string): Uint8Array {
//   const utf8 = [];
//   for (let i = 0; i < str.length; i++) {
//     let charCode = str.charCodeAt(i);
//     if (charCode < 0x80) {
//       utf8.push(charCode); // 单字节字符 (ASCII)
//     } else if (charCode < 0x800) {
//       utf8.push(0xC0 | (charCode >> 6), 0x80 | (charCode & 0x3F)); // 双字节字符
//     } else if (charCode < 0x10000) {
//       utf8.push(0xE0 | (charCode >> 12), 0x80 | ((charCode >> 6) & 0x3F), 0x80 | (charCode & 0x3F)); // 三字节字符
//     }
//   }
//   return new Uint8Array(utf8);
// }
//
// // 定义串口地址选项
// const serialOptions = [
//   { value: '/dev/ttyS0' }, { value: '/dev/ttyS3' },
//   { value: '/dev/ttyS4' }, { value: '/dev/ttyS5' },
//   { value: '/dev/ttyS7' }, { value: '/dev/ttyS8' }
// ];
// // 定义波特率选项
// const baudRateOptions = [
//   { value: '9600' },
//   { value: '19200' },
//   { value: '38400' },
//   { value: '115200' }
// ];
//
//
// @Preview
// @Entry
// @Component
// struct Index {
//   @State port: string = "/dev/ttyS5"
//   @State baudrate: string = "9600"
//   @State sendData: string = "FF 55 01 23 88"
//   @State message: string = ''
//   @State result: string = ''
//   @State isPortOpen: boolean = true;
//   @State openCloseButtonLabel: string = '打开串口';
//   @State receiveInterval: number = 50;
//   @State sendInterval: number = 500;
//   @State timestampEnabled: boolean = true
//   @State isHex: boolean = true
//   @State isTimerSend: boolean = false
//   scroller: Scroller = new Scroller()
//   //定时循环读取
//   timerUartRead() {
//     let isHexMode =this.isHex;
//     let scroller:Scroller =this.scroller;
//     console.warn('TAG_INDEX', 'timerUartRead')
//     // Logger.info(TAG, `start 开始接收串口数据`);
//     if (uartFd != -1) {
//       clearInterval(oTimer)
//       oTimer = setInterval(function () {
//         if (uartFd != -1) {
//           let ret = serialPort.read();
//           if (ret != null) {
//             let receiveData = anyToString(ret);
//             if (isHexMode) {
//               Logger.error(TAG, '转成16进制字符串');
//               let bytes = hexStringToByteArray(receiveData);
//               receiveData = bytesToHexString(bytes);
//             } else {
//               receiveData = hexStringToString(receiveData)
//             }
//             Logger.info(TAG, `uart 接收到的串口数据:` + receiveData);
//             workerInstance.postMessage(receiveData);
//             scroller.scrollEdge(Edge.Bottom)
//           }
//         } else {
//           clearInterval(oTimer)
//           Logger.info(TAG, `stop 停止接收串口数据`)
//         }
//       }, 50) //10ms
//     } else {
//       Logger.info(TAG, `uart 串口还没打开`)
//     }
//   }
//
//   workerUartRx() {
//     Logger.info(TAG, 'workerUartRx  接收串口数据');
//     workerInstance.onmessage = (e) => {
//       let data = e.data
//       if (data != null) {
//         Logger.info(TAG, `接收串口数据:${data}`);
//         this.message += '\n' + `时间：${this.getCurrentTime()}  数据：${data}`;
//         // this.scroller.scrollEdge(Edge.Bottom)
//         //  Logger.info(TAG, `接收数据 HEX:` +  parseInt( data.slice(0,2),16).toString());
//       }
//     }
//   }
//
//   onPageShow() {
//     Logger.info(TAG, '  onPageShow');
//     this.workerUartRx();
//   }
//
//   onPageHide() {
//     Logger.info(TAG, '  onPageHide');
//     if (uartFd != -1) {
//       clearInterval(oTimer);
//       serialPort.close();
//     }
//   }
//
//   send() {
//     if (uartFd == -1) {
//       prompt.showToast({ message: '请先打开串口' })
//       return
//     }
//     if (this.sendData == null && this.sendData == "") {
//       prompt.showToast({ message: '发送数据不能为空' })
//       return
//     }
//     // let bytes = hexStringToUint8Array(this.sendData)
//     let ret: number = serialPort.write(this.sendData);
//     //this.result += "\n"+`写入数据:${this.sendData}${ret >= 0 ? "成功" : "失败"}`;
//
//     this.result += `发送数据:${this.sendData}`;
//     // this.message = this.result + '\n';
//     Logger.info(TAG, `发送结果 = ${ret}`);
//   }
//
//   // 发送数据的改进版本
//   sendDataToPort() {
//     Logger.info(TAG, '  发送数据');
//     if (uartFd == -1) {
//       Logger.info(TAG, '  串口未打开');
//       console.error('串口未打开');
//       prompt.showToast({ message: '请先打开串口' })
//       return;
//     }
//     let ret: number = -1;
//     let msg: string = null
//     if (this.isHex) {
//       // 字符串转换为bytes
//       let dataToSend = hexStringToBytes(this.sendData);
//       ret = serialPort.write(dataToSend);
//       Logger.warn(TAG, `发送数据：${bytesToHexString(dataToSend)}`);
//       msg = `写入数据:${bytesToHexString(dataToSend)}${ret >= 0 ? "成功" : "失败"}`;
//     } else {
//       ret = serialPort.write(this.sendData);
//       Logger.warn(TAG, `发送数据：${this.sendData}`);
//       msg = `写入数据:${this.sendData}${ret >= 0 ? "成功" : "失败"}`;
//     }
//
//     this.result += `发送数据:${this.sendData}`;
//     // this.message = this.result + '\n';
//     Logger.info(TAG, `发送结果 = ${msg}`);
//   }
//
//   // 实时接收串口数据
//   receiveData() {
//     if (uartFd == -1) {
//       console.error('串口未打开');
//       Logger.info(TAG, '  串口未打开');
//       return;
//     }
//
//     this.timerUartRead();
//   }
//
//   getCurrentTime(): string {
//     const currentDate = new Date();
//     // const year = currentDate.getFullYear();
//     // const month = currentDate.getMonth() + 1;
//     // const day = currentDate.getDate();
//     const hours = currentDate.getHours();
//     const minutes = currentDate.getMinutes();
//     const seconds = currentDate.getSeconds();
//     const milliseconds = currentDate.getMilliseconds();
//
//     // const formattedTime = `${year}-${month.toString().padStart(2, '0')}-${day.toString().padStart(2, '0')} ${hours.toString().padStart(2, '0')}:${minutes.toString().padStart(2, '0')}:${seconds.toString().padStart(2, '0')}.${milliseconds}`;
//     const formattedTime = `${hours.toString().padStart(2, '0')}:${minutes.toString().padStart(2, '0')}:${seconds.toString().padStart(2, '0')}.${milliseconds}`;
//
//     console.log(formattedTime);
//     return formattedTime;
//   }
//
//   openSerialPort() {
//     Logger.info(TAG, "打開串口")
//     try {
//       if (uartFd == -1) {
//         Logger.info(TAG, "串口未打開")
//         uartFd = serialPort.open(this.port, parseInt(this.baudrate)); //
//         this.result += "\n" + `串口打开:${uartFd >= 0 ? "成功" : "失败"}`;
//         Logger.info(TAG, `串口已打開serialPort port ${this.port},open result is ${uartFd}`);
//         this.isPortOpen = true;
//         this.openCloseButtonLabel = '关闭串口';
//       } else {
//         Logger.info(TAG, `串口已打開`);
//         this.result += "\n" + `串口已打开`;
//       }
//
//       this.message = this.result;
//
//       // 每50毫秒接收一次数据
//       this.receiveInterval = setInterval(() => {
//         this.receiveData();
//       }, this.receiveInterval);
//     } catch (err) {
//       console.error('打开串口失败: ', err);
//     }
//   }
//
//   closeSerialPort() {
//     Logger.info(TAG, "关闭串口")
//     try {
//       if (uartFd == -1) {
//         prompt.showToast({ message: '请先打开串口' })
//         return
//       }
//       let ret: number = serialPort.close();
//       this.result += "\n" + `串口关闭:${ret >= 0 ? "成功" : "失败"}`;
//       this.message = this.result;
//       Logger.info(TAG, `serialPort close result is ${ret}`);
//       uartFd = -1;
//       this.isPortOpen = false;
//       this.openCloseButtonLabel = '打开串口';
//       clearInterval(this.receiveInterval);
//     } catch (err) {
//       console.error('关闭串口失败: ', err);
//     }
//   }
//
//
//   build() {
//     Column() {
//       Text('接收数据区:')
//         .textAlign(TextAlign.Start)
//         .alignSelf(ItemAlign.Start)
//         .backgroundColor(Color.Pink)
//       Row() {
//         //接收数据区域
//         Column(){
//           Scroll(this.scroller){
//             Text(this.message)
//
//           }
//           .scrollBar(BarState.Off)
//         }
//         .width('70%')
//         .height('100%')
//         .borderWidth(1)
//         .borderRadius(5)
//         .padding(15)
//         .alignItems(HorizontalAlign.Start)
//
//         //右侧功能设置区域
//         Column() {
//           //选择串口号
//           Row() {
//             Text('串口号：')
//               .fontSize(24)
//               .textAlign(TextAlign.Center)
//
//             Select(serialOptions)
//               .selected(0)
//               .value(this.port)
//               .margin(5)
//               .onSelect((index: number, text?: string | undefined) => {
//                 console.info('Select port is :' + text)
//                 this.port = text;
//                 Logger.info(TAG, `This port is ${this.port}`);
//               })
//           }
//           .padding(5)
//           .width('100%')
//
//           //选择波特率
//           Row() {
//             Text('波特率：')
//               .fontSize(24)
//               .textAlign(TextAlign.Center)
//
//             Select(baudRateOptions)
//               .selected(0)
//               .value(this.baudrate)
//               .margin(5)
//               .onSelect((index: number, text?: string | undefined) => {
//                 console.info('Select baudrate is :' + text)
//                 this.baudrate = text;
//                 Logger.info(TAG, `This baudrate is ${this.baudrate}`);
//               })
//           }
//           .padding(5)
//           .width('100%')
//
//           //打开关闭串口按钮
//           Button(this.openCloseButtonLabel, { type: ButtonType.Normal })// {
//             .borderRadius(10)
//             .padding(10)
//             .width('100%')
//             .fontSize(28)
//             .onClick(() => {
//               if (uartFd == -1) {
//                 this.openSerialPort()
//               } else {
//                 this.closeSerialPort()
//               }
//             })
//
//           // 16进制切换
//
//           Row() {
//             Text('16进制')
//             Checkbox()
//               .select(this.isHex)
//               .onChange((value: boolean) => {
//                 this.isHex = value;
//                 if(this.isHex){
//                   this.sendData ='FF 55 01 23 88';
//                 }else{
//                   this.sendData ='AT+V\n';
//                 }
//               })
//           }
//
//           Row() {
//
//             // 定时发送
//             Checkbox()
//               .select(this.isTimerSend)
//               .onChange((value: boolean) => {
//                 this.isTimerSend = value
//               })
//             Text('定时发送')
//             // 间隔发送时间输入框
//             TextInput({ placeholder: '输入发送间隔 (ms)', text: this.sendInterval.toString() })
//               .type(InputType.Number)
//               .onChange((value: string) => {
//                 this.sendInterval = parseInt(value)
//               })
//           }
//
//           //清空数据
//           Button('清空接收数据', { type: ButtonType.Normal })
//             .fontSize(28)
//             .padding(10)
//             .width('100%')
//             .borderRadius(10)
//             .onClick(() => {
//               this.message = '';
//               this.result = '';
//               Logger.info(TAG, `clean text data`);
//             })
//
//         }
//         .width('28%')
//         .height('80%')
//       }
//       .height('70%')
//       .width('100%')
//
//       Text('发送数据区:')
//         .alignSelf(ItemAlign.Start)
//         .backgroundColor(Color.Pink)
//       //底部发送区域
//       Row() {
//         TextInput({ placeholder: '请输入发送数据(默认:' + this.sendData + ')', text: `${this.sendData}` })
//           .fontSize(24)
//           .margin(5)
//           .padding(10)
//           .borderRadius(12)
//           .onChange((value: string) => {
//             this.sendData = value;
//             Logger.info(TAG, `This send data is ${this.sendData}`);
//           })
//           .width('70%')
//           .height('100%')
//           .borderWidth(1)
//           .alignSelf(ItemAlign.Start)
//           .align(Alignment.Start)
//
//         Button('发送', { type: ButtonType.Normal })
//           .fontWeight(FontWeight.Bold)
//           .fontSize(30)
//           .height('80%')
//           .width('25%')
//           .borderRadius(10)
//           .onClick(() => {
//             this.sendDataToPort()
//           })
//       }
//       .width('100%')
//       .height('20%')
//     }
//     .width('100%')
//     .height('100%')
//
//   }
// }
//
//
//
//
//
//
//
//
//
//
