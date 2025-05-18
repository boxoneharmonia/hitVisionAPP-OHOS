declare namespace burner {

  function StartBurning(threads: number);

  function StopBurning();

}

export default burner

// export const OpenSerial: (port: string, baudrate: number) => number;
//
// export const WriteSerial: (fd: number, data: string | Uint8Array) => number;
//
// export const ReadSerial: (fd: number) => Uint8Array;
//
// export const CloseSerial: (fd: number) => number;
