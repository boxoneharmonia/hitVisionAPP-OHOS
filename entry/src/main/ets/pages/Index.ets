import hilog from '@ohos.hilog';

@Entry
@Component
struct Index {
  @State message: string = 'Hello World';

  build() {
    Row() {
      Column() {
        Text(this.message)
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
            hilog.info(0x0000, 'testTag', 'Test NAPI');
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}
