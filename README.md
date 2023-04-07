# DetectionGenshinPlayerUID
通过截图识别UID并检测玩家UID是否在黑名单列表内并发出声音提示  
# 具体流程  
1.截取UID所在的屏幕区域图片  
![image](https://user-images.githubusercontent.com/52804763/230605357-b9876b49-97e8-4346-9404-3f303b8c41fb.png)  
2.将截到的UID图片把白色相近的颜色提取，其他颜色删除避免识别出错  
![image](https://user-images.githubusercontent.com/52804763/230605454-2963ae5a-2e0b-4047-9bfb-e6c753ef85c0.png)  
3.将UID与已记录UID进行比对之后发出对应的提示音  
# 如何使用
下载成品tesseract安装之后把文件像下图一样放置即可(https://digi.bib.uni-mannheim.de/tesseract)
![image](https://user-images.githubusercontent.com/52804763/230606207-42d036e1-310d-4ef0-bd80-02bea25c8245.png)  
  
  
# 使用项目列表
tesseract from https://github.com/tesseract-ocr/tesseract  
tesseract.Release from https://digi.bib.uni-mannheim.de/tesseract  
ini from https://github.com/rxi/ini  
