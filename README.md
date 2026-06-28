# Acrodea Runtime

## 简介

G-GEE 游戏商店启动器 （psvita 安卓游戏移植项目）

## 支持平台

* Sony Ps Vita

## 如何编译

1. 确保已安装 [vitasdk-softfp](https://github.com/vitasdk-softfp) 并克隆项目

2. 无调试模式：

```bash
mkdir build && cmake .. && make
```

3. 带调试模式:

```bash
mkdir build && cmake .. -DCMAKE_BUILD_TYPE=Debug && make
```

## 依赖

- [VitaSDK](https://vitasdk.org/)
- [kubridge.skprx](https://github.com/bythos14/kubridge)
- [libshacccg.suprx](https://github.com/Rinnegatamante/ShaRKBR33D)

## 开发人员

* 初代NPC
* 一直改
* SpriteLisen

## 特别致谢

* KAAAsS 技术帮助

## 致谢

- [Vitasdk](https://github.com/vitasdk)
- [TheFloW](https://github.com/TheOfficialFloW) — 原始 so_util 实现
- [Rinnegatamante](https://github.com/Rinnegatamante) — vitaGL 等贡献
- [v-atamanenko](https://github.com/v-atamanenko) — soloader-boilerplate 参考
- 大川、猎户兄 — 提供游戏源文件
- ...

> 若有遗漏，敬请提醒补上。
