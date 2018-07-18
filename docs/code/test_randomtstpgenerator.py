import time
import random

for i in range(1, 30):
    epoch_time = int(time.time())
    strdate = format(epoch_time, 'x').zfill(8);
    uid = random.randint(0, 65536*65536)
    struid = format(uid, 'x').zfill(8);
    print(strdate + struid)
    time.sleep(1)
