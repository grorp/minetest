const fs = require('node:fs');

const data = fs.readFileSync('debug extract.txt', 'utf8');
const arr = data.replaceAll(/.*ACTION\[Server\]: /g, "").replaceAll(/.*ACTION\[Main\]: /g, "").split("\n")
let new_arr = [];
for (const item of arr) {
    if (new_arr[new_arr.length - 1]?.str == item) {
        new_arr[new_arr.length - 1].count++;
    } else {
        new_arr.push({ str: item, count: 1 });
    }
}
let new_arr_2 = [];
for (const item of new_arr) {
    if (item.count == 1) {
        new_arr_2.push(item.str);
    } else {
        new_arr_2.push(item.str + " (×" + item.count + ")");
    }
}
fs.writeFileSync('debug extract processed.txt', new_arr_2.join("\n"));
