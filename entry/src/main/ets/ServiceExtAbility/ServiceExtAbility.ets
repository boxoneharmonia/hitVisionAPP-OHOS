import ServiceExtensionAbility from '@ohos.app.ability.ServiceExtensionAbility';
import ethernet from '@ohos.net.ethernet';
import hilog from '@ohos.hilog';
import serialPort1 from "libserialPort1.so"

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

export default class ServiceExtAbility extends ServiceExtensionAbility
{
  onCreate() {
    hilog.info(0x0000, 'SetIP', 'ServiceAbility onCreate');
    trySetIface("eth0", config0);
    trySetIface("eth1", config1);
    serialPort1.startSp1();
  }

  onRequest() {

  }

  onDestroy() {
    serialPort1.stopSp1();
  }
}