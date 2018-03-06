import axios from 'axios';
import childProcess from 'child_process';
import { createDir, createFile } from './utils/fs';
import { startServer } from './utils/server';

var test = require('tape');
const { PORT, DOC_ROOT } = require('../config.json');

const SERVER_ENDPOINT = `http://localhost:${PORT}`;


const TEST_FILES = ['TEST_FILE1.txt', 'TEST_FILE2.txt', 'TEST_FILE3.txt'];
const TEST_CONTENTS = ['TEST_CONTENT1', 'TEST_CONTENT2', 'TEST_CONTENT3'];
test('[server: ] can serve plain text file to axios client', async function (t) {
    t.plan(2 * TEST_FILES.length);
    let serverProc = null;
    try {
      serverProc = await startServer();
      if (!serverProc) {
        t.fail('failed to startServer');
      }
    } catch(e) {
      t.fail(`failed to prepare test, e: ${e.message}`);
    }

    serverProc.stdout.on('data', (msg) => {
      console.log('serverProc.stdout: ', msg.toString());
    });

    for (let ii = 0; ii < TEST_FILES.length; ii++) {
      const TEST_FILE = TEST_FILES[ii];
      const TEST_CONTENT = TEST_CONTENTS[ii];

      await createFile(DOC_ROOT, TEST_FILE, TEST_CONTENT);
      const fileUrl = `${SERVER_ENDPOINT}/${TEST_FILE}`;
      try {
        const { status, data } = await axios.get(fileUrl);
        t.equal(Math.floor(status / 100), 2, "status code should be 2xx");
        t.equal(data, TEST_CONTENT, "resp data is valid");
      } catch(e) {
        t.fail(`failed to load url ${fileUrl}, error: ${e.message}`);
      }
    }

    if (serverProc) {
      serverProc.kill('SIGTERM');
    }
});
