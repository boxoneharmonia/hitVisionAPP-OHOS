import ServiceExtensionAbility from '@ohos.app.ability.ServiceExtensionAbility';
import ethernet from '@ohos.net.ethernet';
import hilog from '@ohos.hilog';
import serial from "libserialport.so"  // serial.cpp编译形成的动态链接库.so文件
import SystemCpuUsage from "libSystemCpuUsage.so"
import burner from "libburner.so"

const config0: ethernet.InterfaceConfiguration = {
  mode: ethernet.IPSetMode.STATIC,
  ipAddr: "192.168.0.233",
  route: "192.168.0.1",
  gateway: "192.168.0.1",
  netMask: "255.255.255.0",
  dnsServers: "0.0.0.0"
};

const config1: ethernet.InterfaceConfiguration = {
  mode: ethernet.IPSetMode.STATIC,
  ipAddr: "192.168.0.234",
  route: "192.168.0.1",
  gateway: "192.168.0.1",
  netMask: "255.255.255.0",
  dnsServers: "0.0.0.0"
};

function trySetIface(iface: string, config: ethernet.InterfaceConfiguration) {
  ethernet.isIfaceActive(iface).then(() => {
    ethernet.setIfaceConfig(iface, config).then(() => {
      hilog.info(0x0000, 'SetIP', `Set ${iface} ok`);
    }).catch((err) => {
      hilog.error(0x0000, 'SetIP', `Set ${iface} err: %{public}s`, JSON.stringify(err));
    });
  }).catch((err) => {
    hilog.warn(0x0000, 'SetIP', `${iface} unlinked: %{public}s`, JSON.stringify(err));
  });
}



// function decimalToHexadecimal(decimal: number): string {
//   let hexadecimal = "";
//   const hexDigits = "0123456789ABCDEF";
//   // 通过取余运算进行转换
//   while (decimal > 0) {
//     hexadecimal = hexDigits[decimal % 16 ] + hexadecimal;
//     decimal = Math.floor(decimal / 16);
//   }
//   return hexadecimal;
// }

export default class ServiceExtAbility extends ServiceExtensionAbility
{
  port: string = "/dev/ttyS0"
  baudrate: number = 9600
  //sendData: string = "Initialized"
  message: string = ''
  result: string = ''
  isPortOpen: boolean = true;
  receiveInterval: number = 500;
  sendInterval: number = 1000;
  timestampEnabled: boolean = true
  isTimerSend: boolean = false
  uartFd: number = -1;
  oTimer: number = 0;

  // uartTime: number = 0;  // 串口工作时间（以秒为单位）
  // sysCpuUsage: number = 0;  // 系统cpu占用率

  onCreate() {
    hilog.info(0x0000, 'SetIP', 'ServiceAbility onCreate');
    trySetIface("eth0", config0);
    trySetIface("eth1", config1);

    // 这里需要写入读取串口位置的代码
    // let port: string = "xxxxx"

    try {
      if (this.uartFd == -1) {
        hilog.info(0x0000, 'OpenUart', '正在打开串口 Openning UART');
        this.uartFd = serial.OpenSerial(this.port, this.baudrate); //
        hilog.info(0x0000, 'OpenUart', '串口打开成功 UART opened successfully');
        this.isPortOpen = true;
      } else {
        hilog.info(0x0000, 'OpenUart', '串口已打开 UART is already open');
        this.result += "\n" + `串口已打开`;
      }

      // 每50毫秒接收一次数据?暂时不考虑
      /*this.receiveInterval = setInterval(() => {
        this.receiveData();
      }, this.receiveInterval);*/
    } catch (err) {
      console.error('打开串口失败: ', err);
    }

  }

  onRequest() {

    hilog.info(0x0000, 'UartService', '串口数据%{public}d 传输开始 Beginning UART data transfer',this.uartFd)
    if (this.uartFd != -1) {
      clearInterval(this.oTimer)
      let Fd = this.uartFd;
      // hilog.info(0x0000, 'UartService', '待写入串口: %{public}d', Fd)
      let receiveData: Uint8Array = new Uint8Array([128, 150]);
      let uartTime: number = 0;
      let sysCpuUsage: number = 0;  // 系统cpu占用率
      let Interval = this.sendInterval/1000;
      // hilog.info(0x0000, 'UartService', 'Uint8初始数据： %{public}d %{public}d', receiveData[0], receiveData[1])

      this.oTimer = setInterval(function () {
        if (Fd != -1) {

          sysCpuUsage = SystemCpuUsage.SysCpuUsage() * 100;
          //字符串输出：
          //sendData = 'runtime: '+uartTime.toString()+'s ; cpu occupation: '+sysCpuUsage.toString()+'%';
          //标准十六进制数输出： 1ACF+6位时间+4位cpu占用率
          let sendData = new Uint8Array(7);
          sendData[0] = 0x1A;
          sendData[1] = 0xCF;
          sendData[2] = (uartTime>>16) & 0xFF;
          sendData[3] = (uartTime>>8) & 0xFF;
          sendData[4] = uartTime & 0xFF;
          sendData[5] = 0x00;
          sendData[6] = Math.floor(sysCpuUsage) & 0xFF;
          // sendData = '1ACF'+decimalToHexadecimal(uartTime).padStart(6, '0')+decimalToHexadecimal(Math.floor(sysCpuUsage)).padStart(4, '0')
          // hilog.info(0x0000, 'UartService', '正在尝试写入串口%{public}d Attempting to write into UART...', Fd)
          hilog.info(0x0000, 'UartService', '正在尝试写入串口%{public}d,%{public}d,%{public}d,%{public}d,%{public}d,%{public}d,%{public}d.',
            sendData[0],sendData[1],sendData[2],sendData[3],sendData[4],sendData[5],sendData[6])
          let ret = serial.WriteSerial(Fd, sendData);
          if (ret != null) {
            hilog.info(0x0000, 'UartService', '串口发送%{public}d byte(s)', ret);
          }

          // hilog.info(0x0000, 'UartService', '正在尝试读取串口%{public}d Attempting to read UART...', Fd)
          receiveData = serial.ReadSerial(Fd);

          // const hexString = Array.from(receiveData)
          //   .map(byte => byte.toString(16).padStart(2, '0').toUpperCase())
          //   .join(' ');
          hilog.info(0x0000, 'UartService', '收到串口通信数据 received data from UART:%{public}d, %{public}d', receiveData[0], receiveData[1])
          if (receiveData[0] == 170 && receiveData[1] == 0)
          {
            hilog.info(0x0000, 'StressTest', '正在启动CPU压力测试 Starting CPU stress test...')
            burner.StartBurning(4);
            hilog.info(0x0000, 'StressTest', 'CPU压力测试已启动 CPU stress test has been started.')
          }
          else if (receiveData[0] == 0 && receiveData[1] == 187)
          {
            hilog.info(0x0000, 'StressTest', '正在结束CPU压力测试 Terminating CPU stress test...')
            burner.StopBurning();
            hilog.info(0x0000, 'StressTest', 'CPU压力测试已结束 CPU stress test has been terminated.')
          }

          uartTime += Interval;
        } else {
          clearInterval(this.oTimer)
          hilog.info(0x0000, 'DataSend', '串口已关闭，写入失败 Data sending stopped, as UART has been closed');
        }
      }, this.sendInterval) //定时器，每隔一个sendInterval的时间发送一次数据
    } else {
      hilog.info(0x0000, 'DataSend', '串口未开 UART has ont been opened yet');
    }
  }

  onDestroy() {
    hilog.info(0x0000, 'Close UART', '正在关闭串口 Closing UART');
    try {
      let Fd = this.uartFd;
      hilog.info(0x0000, 'Close UART', '待关闭串口: %{public}d', Fd)
      let ret: number = serial.CloseSerial(Fd);
      hilog.info(0x0000, 'Close UART', '串口关闭结果为 serialPort close result is %{public}d', ret);
      this.uartFd = -1;
      this.isPortOpen = false;
      // clearInterval(this.receiveInterval);
    } catch (err) {
      console.error('关闭串口失败: ', err);
    }
  }
}