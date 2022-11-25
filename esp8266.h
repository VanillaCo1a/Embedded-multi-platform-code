#ifndef __ESP8266_H
#define __ESP8266_H

#if defined(_WIN32) || defined(_WIN64)
/*****    windows testing part    *****/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <math.h>

#else
#include "device.h"

#endif

#define MAXNUM_TOPIC 10

typedef struct {
    /* 模式 */
    /*  0: 无 Wi-Fi 模式，并且关闭 Wi-Fi RF
        1: Station 模式
        2: SoftAP 模式
        3: SoftAP+Station 模式 */
    uint8_t mode;
    /* 切换 ESP 设备的 Wi-Fi 模式时（例如，从 SoftAP 或无 Wi-Fi 模式切换为 Station 模式或 SoftAP+Station 模式），是否启用自动连接 AP 的功能，默认值：1。参数缺省时，使用默认值，也就是能自动连接。 */
    /*  0: 禁用自动连接 AP 的功能
    1: 启用自动连接 AP 的功能，若之前已经将自动连接 AP 的配置保存到 flash 中，则 ESP 设备将自动连接 AP */
    uint8_t autoConnect;
    /* 目标 AP 的 SSID */
    const char *ssid;
    /* 密码最长 64 字节 ASCII */
    const char *pwd;
    /* 目标 AP 的 MAC 地址，当多个 AP 有相同的 SSID 时，该参数不可省略 */
    char *bssid;
    /* 信道号 */
    uint8_t channel;
    /* PCI 认证 */
    /*  0: ESP station 可与任何一种加密方式的 AP 连接，包括 OPEN 和 WEP
        1: ESP station 可与除 OPEN 和 WEP 之外的任何一种加密方式的 AP 连接 */
    uint8_t pciEn;
    /* Wi-Fi 重连间隔，单位：秒，默认值：1，最大值：7200 */
    /*  0: 断开连接后，ESP station 不重连 AP
    [1,7200]: 断开连接后，ESP station 每隔指定的时间与 AP 重连 */
    uint8_t reconnInterval;
    /* 监听 AP beacon 的间隔，单位为 AP beacon 间隔，默认值：3，范围：[1,100] */
    uint8_t listenInterval;
    /* 扫描模式 */
    /*  0: 快速扫描，找到目标 AP 后终止扫描，ESP station 与第一个扫描到的 AP 连接
        1: 全信道扫描，所有信道都扫描后才终止扫描，ESP station 与扫描到的信号最强的 AP 连接 */
    uint8_t scanMode;
    /* AT+CWJAP 命令超时的最大值，单位：秒，默认值：15，范围：[3,600] */
    uint16_t jap_timeout;
    /* PMF（Protected Management Frames，受保护的管理帧），默认值 0 */
    /*  0 表示禁用 PMF
        bit 0: 具有 PMF 功能，提示支持 PMF，如果其他设备具有 PMF 功能，则 ESP 设备将优先选择以 PMF 模式连接
        bit 1: 需要 PMF，提示需要 PMF，设备将不会关联不支持 PMF 功能的设备 */
    uint8_t pmf;
} ESP8266_WifiTypedef;

typedef struct {
    /* 发布/订阅的 topic, 最大长度：128 字节 */
    char *topic[MAXNUM_TOPIC];
    /* 发布/订阅的 QoS, 参数可选 0、1、或 2, 默认值：0 */
    uint8_t qos[MAXNUM_TOPIC];
    /* 发布 retain */
    uint8_t retain[MAXNUM_TOPIC];
    /* 订阅的 topic 的 requestid, 进行回复后请清空 */
    char requestId[MAXNUM_TOPIC][128];
    /* MQTT 缓冲区指针 */
    void *buffer;

    /* 当前有效的 topic 数量 */
    uint8_t num;
} ESP8266_MqttTopicTypedef;

typedef struct {
    /* 当前仅支持 link ID 0 */
    uint8_t linkId;
    /* 由于 ESP8266 内存限制, 不支持 MQTT over TLS, 即只能取 1 或 6 */
    /*  1: MQTT over TCP;                       2: MQTT over TLS(不校验证书);
        3: MQTT over TLS(校验 server 证书);      4: MQTT over TLS(提供 client 证书);
        5: MQTT over TLS(校验 server 证书并且提供 client 证书);
        6: MQTT over WebSocket(基于 TCP);        7: MQTT over WebSocket Secure(基于 TLS, 不校验证书);
        8: MQTT over WebSocket Secure(基于 TLS, 校验 server 证书);
        9: MQTT over WebSocket Secure(基于 TLS, 提供 client 证书);
        10: MQTT over WebSocket Secure(基于 TLS, 校验 server 证书并且提供 client 证书)*/
    uint8_t scheme;
    /* 证书 ID, 目前 ESP-AT 仅支持一套 cert 证书, 参数为 0 */
    uint8_t certKeyId;
    /* CA ID, 目前 ESP-AT 仅支持一套 CA 证书, 参数为 0 */
    uint8_t CAId;
    /* 资源路径, 最大长度：32 字节 */
    char *path;

    /* 遗嘱 topic, 最大长度：128 字节 */
    char *lwt_topic;
    /* 遗嘱 message, 最大长度：64 字节 */
    char *lwt_msg;
    /* 遗嘱 QoS, 参数可选 0、1、2, 默认值：0 */
    uint8_t lwt_qos;
    /* 遗嘱 retain, 参数可选 0 或 1, 默认值：0 */
    uint8_t lwt_retain;

    /* MQTT ping 超时时间, 单位：秒, 范围：[0,7200], 默认值：0, 会被强制改为 120 秒 */
    uint16_t keepalive;
    /* 设置 MQTT 清理会话标志, 有关该参数的更多信息请参考 MQTT 3.1.1 协议中的 Clean Session 章节 */
    bool disableCleanSession;
    /* 0: MQTT 不自动重连; 1: MQTT 自动重连, 会消耗较多的内存资源 */
    bool reconnect;

    /* MQTT broker 域名, 最大长度：128 字节 */
    char *host;
    /* MQTT broker 端口, 最大端口：65535 */
    uint16_t port;

    /* MQTT 客户端 ID, 最大长度：256 字节 */
    char *clientId;
    /* 用户名, 用于登陆 MQTT broker, 最大长度：64 字节 */
    char *username;
    /* 密码, 用于登陆 MQTT broker, 最大长度：64 字节 */
    char *password;

    /* MQTT 发布的主题 */
    // ESP8266_MqttTopicTypedef publish;

    /* MQTT 订阅的主题 */
    ESP8266_MqttTopicTypedef subscribe;

    /* MQTT 状态： */
    /*  0: MQTT 未初始化;                   1: 已设置 AT+MQTTUSERCFG;
        2: 已设置 AT+MQTTCONNCFG;           3: 连接已断开;
        4: 已建立连接;                      5: 已连接, 但未订阅 topic;
        6: 已连接, 已订阅过 topic  */
    uint8_t state;
} ESP8266_MqttTypedef;

typedef struct {
    //todo: add tcp cfg
} ESP8266_TcpTypedef;

typedef struct {
    ESP8266_WifiTypedef wifi;
    ESP8266_TcpTypedef tcp;
    ESP8266_MqttTypedef mqtt;
} ESP8266_Typedef;

void ESP8266_WifiInit(ESP8266_Typedef *esp8266, uint8_t mode, const char *ssid, const char *pwd);
void ESP8266_MqttConnectInit(ESP8266_Typedef *esp8266, char *hostaddr, int hostport);
void ESP8266_MqttUserInit(ESP8266_Typedef *esp8266,
                          char *clientid, char *username, char *password);
void ESP8266_MqttPubSubInit(ESP8266_Typedef *esp8266);
// int ESP8266_MqttPubTopicInit(ESP8266_Typedef *esp8266, char *topicPublish[], int qosPublish[], int sizePublish);
int ESP8266_MqttSubTopicInit(ESP8266_Typedef *esp8266, char *topicSubscribe[], int qosSubscribe[],
                             int sizeSubscribe, void *topicBuffer);

int ESP8266_WifiConnect(ESP8266_Typedef *esp8266);
int ESP8266_MqttUserConfig(ESP8266_Typedef *esp8266);
int ESP8266_MqttConnectConfig(ESP8266_Typedef *esp8266);
int ESP8266_MqttConnect(ESP8266_Typedef *esp8266);
int ESP8266_MqttDisconnect(ESP8266_Typedef *esp8266);
int ESP8266_MqttPublish(ESP8266_Typedef *esp8266, char *topicPublish, int qosPublish, void *topicBuffer);
int ESP8266_MqttSubscribe(ESP8266_Typedef *esp8266);
int ESP8266_MqttUnsubscribe(ESP8266_Typedef *esp8266);
int ESP8266_MqttSubRes(ESP8266_Typedef *esp8266);

#endif    // !__ESP_8266_H
