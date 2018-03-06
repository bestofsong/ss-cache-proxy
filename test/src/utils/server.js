import childProcess from 'child_process';

var CONFIG = require('../../config.json');


const { HOST, PORT, DOC_ROOT, SERVER_BIN } = CONFIG;


export async function startServer() {
  const proc = childProcess.spawn(SERVER_BIN, [HOST, PORT, DOC_ROOT]);
  return new Promise((resolve) => {
    resolve(proc);
  });
}

