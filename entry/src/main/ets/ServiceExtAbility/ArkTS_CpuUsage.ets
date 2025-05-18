// 导入必要模块
// import fs from '@ohos.file.fs';
// import util from '@ohos.util';
//
// // 定义CPU时间数据结构
// interface CpuTime {
//   total: number;
//   idle: number;
// }
//
// // 获取CPU时间数据的异步函数
// async function getCpuTime(): Promise<CpuTime> {
//   try {
//     // 打开/proc/stat文件
//     const file = await fs.open('/proc/stat', fs.OpenMode.READ_ONLY);
//
//     // 读取文件内容
//     const buffer = new ArrayBuffer(1024);
//     const readResult = await fs.read(file.fd, buffer, {
//       offset: 0,
//       length: buffer.byteLength
//     });
//
//     // 关闭文件
//     await fs.close(file.fd);
//
//     // 将buffer转换为Uint8Array并进行解码
//     const slicedBuffer = buffer.slice(0, readResult.bytesRead);
//     const uint8Array = new Uint8Array(slicedBuffer);
//     const textDecoder = new util.TextDecoder('utf-8');
//     const content = textDecoder.decode(uint8Array);
//
//     // 解析第一行CPU数据
//     const cpuLine = content.split('\n').find(line => line.startsWith('cpu '));
//     if (!cpuLine) throw new Error('Invalid /proc/stat format');
//
//     const columns = cpuLine.split(/\s+/).slice(1, 8).map(Number);
//     const [user, nice, system, idle, iowait, irq, softirq] = columns;
//
//     return {
//       total: user + nice + system + idle + iowait + irq + softirq,
//       idle: idle + iowait
//     };
//   } catch (error) {
//     console.error('Error reading CPU stats:', error);
//     return { total: 0, idle: 0 };
//   }
// }
// // 计算CPU使用率的函数
// export async function calculateCpuUsage(): Promise<number> {
//   // 第一次采样
//   const startTime = Date.now();
//   const start = await getCpuTime();
//
//   // 等待1秒
//   await new Promise(resolve => setTimeout(resolve, 1000));
//
//   // 第二次采样
//   const end = await getCpuTime();
//   const endTime = Date.now();
//
//   // 计算时间差
//   const totalDiff = end.total - start.total;
//   const idleDiff = end.idle - start.idle;
//
//   // 计算使用率（百分比）
//   if (totalDiff === 0) return 0;
//   const usage = ((totalDiff - idleDiff) / totalDiff) * 100;
//
//   return Number(usage.toFixed(1));
// }
//
// // 使用示例
// async function main() {
//   const cpuUsage = await calculateCpuUsage();
//   console.log(`Current CPU Usage: ${cpuUsage}%`);
// }
//
// main();