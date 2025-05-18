declare namespace serial {

  function OpenSerial(port: string, baudrate: number): number;


  function WriteSerial(fd: number, data: string | Uint8Array): number;


  function ReadSerial(fd: number): Uint8Array;


  function CloseSerial(fd: number): number;
}

export default serial

// export const OpenSerial: (port: string, baudrate: number) => number;
//
// export const WriteSerial: (fd: number, data: string | Uint8Array) => number;
//
// export const ReadSerial: (fd: number) => Uint8Array;
//
// export const CloseSerial: (fd: number) => number;
