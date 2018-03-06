import childProcess from 'child_process';
import http from 'http';
import { createFile, fsReadFile } from './utils/fs';

var test = require('tape');
const { DOC_ROOT } = require('../config.json');


const HOST = 'localhost';
const PORT = 8687;
const TARGET = '/test/file.txt';
const CLIENT_BIN = './../test/client';

function startServer() {
  const server = http.createServer(async function(req, res) {
    const target = req.url;
    const path = `${DOC_ROOT}/${target}`;
    const data = await fsReadFile(path);
    res.setHeader('Content-Type', 'text/plain');
    res.writeHead(200);
    res.end(data);
  });
  server.listen(PORT);
  return server;
}


const TEST_FILE = TARGET.slice(1);
const TEST_CONTENT = 'TEST_CONTENT';
test('[client: ] can request from a node http server', async function (t) {
    t.plan(1);

    const server = startServer();
    await createFile(DOC_ROOT, TEST_FILE, TEST_CONTENT);

    let clientProc = null;
    try {
      clientProc = childProcess.spawn(CLIENT_BIN, [HOST, PORT, TARGET, TEST_CONTENT]);
      if (!clientProc) {
        t.fail('failed to startServer');
      }
    } catch(e) {
      t.fail(`failed to prepare test, e: ${e.message}`);
    }


    clientProc.on('exit', (code) => {
      server.close();
      t.equal(code, 0);
    });
});
