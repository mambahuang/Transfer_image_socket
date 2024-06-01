# NCKU AES Final Project

## Main Function
```shell=
python3 main.py
```

## Addinal Functions using in main.py
> aes.py

> graffiti.py

> UI.py

> image_padding.py

## Main Function (Client Side)
```shell=
python3 main_client.py
```

## Addinal Function using in main_client.py
> merge_file.py

> re_formaatFile.py

## Program for transfering image data
> server_multithread.py

> client_multithread.py

## Things Do in main.py
簽章 → Confirm sign → Save sign →

Padding sign to 1080*1920 image →

split into b, g, r channels → write to b, g, r_channel_alt.h

## Things Do in aes.py
- 用UI介面使用者所輸入的密碼加密手寫簽名
- 產生
    - plaintext_r, g, b.dat
    - key.dat
    - ciphertext_r, g, b.dat
    - decrypted_r, g, b.dat (for verify)

## Things Do in server_multithread.py & client_multithread.py
Run server → wait for client's request... →

If request, sever will see what segment of data client wants →

Start Transfering data → Client receives data

→ Merge & re-format

## Things Do in merge.py (run on client)
- Merge 4 file of 1 channel into original file

## Things Do in re_formatFile.py (run on client)
- create cipher_b, g, r_test.h (for Vitis)

## Final Thing to Do
- Decrypt on hardware with the same key

