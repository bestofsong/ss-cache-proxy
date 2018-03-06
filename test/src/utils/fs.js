import util from 'util';
import path from 'path';
import fs from 'fs';

const fsMkdir = util.promisify(fs.mkdir);
const fsExists = util.promisify(fs.exists);
const fsWriteFile = util.promisify(fs.writeFile);

export const fsReadFile = util.promisify(fs.readFile);


export async function createDir(dir) {
  const exists = await fsExists(dir);
  if (exists) {
    return;
  }
  const parent = dir.replace(/\/[^/]+$/, '');
  if (parent) {
    await createDir(parent);
  }
  await fsMkdir(dir);
}


export async function createFile(dir, name, content) {
  const file_path = `${dir}/${name}`;
  await createDir(path.dirname(file_path));
  await fsWriteFile(file_path, content);
}

