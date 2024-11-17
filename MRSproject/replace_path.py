import os

fp = "./obj"

for dirpath, dirnames, filenames in os.walk(fp):
    for filepath in filenames:
        fname = os.path.join(dirpath, filepath).replace('\\', '/')
        if fname[-9:] == "subdir.mk" or fname[-8:] == "makefile":
            with open(fname, "r") as f:
                data = f.read()
                start_pos = 0
                while True:
                    p = data.find('MRSproject\\', start_pos)
                    if p == -1:
                        break
                    start_pos = p + len("MRSproject")
                    c_pos = data.rfind('"', 0, p)
                    s_pos = data.find(' ', start_pos)
                    data = data[:c_pos] + "\".." + data[start_pos:s_pos].replace('\\', '/') + data[s_pos:]
                    start_pos = c_pos
                with open(fname, 'w') as fo:
                    fo.write(data)
