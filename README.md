## 场景简介
1. 这是使用`DirectInput`下发`力效果`给`下位机`的`demo`；

## 需求
1. 使用`DirectInput`接口，识别一个`下位机`(MOZA R16 WheelBase)  
2. 使用`DirectInput`接口，创建一个`ConstantForce Effect`，`周期`为1秒，`持续时间`为2.5个周期，力效果的`最大扭矩`为下位机最大扭矩的10%，力随时间的变化而呈现出无负方向的正弦状态
3. 使用`DirectInput`接口，将创建的`ConstantForect Effect`下发至`下位机`

## 实现
1. 使用C++语言编写
2. 使用CMAKE工具构建工程
3. 使用Qt搭建app窗口
4. 使用DirectInput8实现力效果
5. 使用MOZA R16基座作为下位机

## 步骤
1. 先用Qt Wiget绘制一个窗口
2. 创建并初始化`DirectInput8`的`接口实例`
3. 使用`接口实例`的`EnumDevices`方法枚举所有连接的力反馈设备
4. 在所有连接设备当中找到目标设备（MOZA R16）
5. 使用`接口实例`的`CreateDevice`创建目标设备的`设备实例`
6. 将`设备实例`的准备工作做好：设置`数据格式`、设置`操作特性`（app的窗口失焦，停止下发力效果）、`程序`获取`设备所有权`
7. 使用`接口实例`的`CreateEffect`方法创建`力效果`，并下发至下位机
8. 使用`力效果`的`SetParameters`方法，周期性的改变力效果参数，达到需求


## Scene introduction
1. This is a `demo` that use of `DirectInput` to deliver `Effect` to `Target Device`;

## Needs
1. Using the `DirectInput` interface, identify a `Target Device` (MOZA R16 WheelBase)
2. Using the `DirectInput` interface, create a `ConstantForce Effect` with a period of 1 second and a duration of 2.5 periods. The maximum torque of the force effect is 10% of the maximum torque of the target Device, and the force is sinusoidal with no negative direction as time changes
3. Use the `DirectInput` interface to create the `ConstantForect Effect` and send it to the `Target Device`

## Realize
1. Write in C++ language
2. Build the project using the CMAKE tool 
3. Use Qt to build the app window
4. Use DirectInput8 to implement the Constant Force Effect
5. Use the MOZA R16 wheelBase as the target device 

## Steps
1. First draw a app window with Qt Widget
2. Create and initialize `DirectInput8 interface instance`
3. Enumerate all connected force-feedback devices using the `DirectInput8 interface instance`'s `EnumDevices method`
4. Locate the `target device` among all connected devices (MOZA R16)
5. Use `CreateDevice` method of Interface Instance to create a `device instance` of the `target device`
6. Do a good job of preparation for the `device instance` : set the `data format`, set the `operation characteristics` (app window out of focus, stop the force effect),`app`to obtain the`target device ownership`
7. Use `interface instance`'s `CreateEffect` method to create `Effect`, and send to the next machine
8. Use `Effect`'s `SetParameters` method, periodically change the Effect parameters, to meet the demand