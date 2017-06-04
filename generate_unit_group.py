data = []
expoent = 0
y = 1

for expoent in range(0, 256):
    if (expoent == 128):
        data.append([expoent, 0])
    else:
        data.append([expoent, y])
    y = y*45 % 257

lookup_exp = "{\n"
for i in range(0, 256):
    lookup_exp = lookup_exp + "\t" + str(data[i][1]) + ",\n"
lookup_exp = lookup_exp + "}"
print(lookup_exp)

data = sorted(data, key=lambda x: x[1])

lookup_log = "{\n"
for i in range(0, 256):
    lookup_log = lookup_log + "\t" + str(data[i][0]) + ",\n"
lookup_log = lookup_log + "}"
print(lookup_log)
