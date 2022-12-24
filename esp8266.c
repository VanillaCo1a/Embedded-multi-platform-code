#include "esp8266.h"


/*****    ESP8266外部调用接口    *****/

static DEVS_TypeDef *esp8266s = NULL;
static DEV_TypeDef *esp8266 = NULL;

/* ESP8266构造函数 */
void ESP8266_Init(DEVS_TypeDef *devs, DEV_TypeDef dev[], poolsize devSize) {
    esp8266s = devs;
    esp8266 = dev;

    /* 初始化设备类和设备, 将参数绑定到设备池中, 并初始化通信引脚 */
    DEV_Init(esp8266s, esp8266, devSize);

    /* 初始化ESP8266设备 */
    ESP8266_DevInit();
}

/* TODO: ESP8266析构函数 */
void ESP8266_Deinit(DEVS_TypeDef *devs, DEV_TypeDef dev[], poolsize size) {}

void ESP8266_DevInit(void) {}

bool ESP8266_Read(uint8_t *data, size_t size) {
    DEV_SetActStream(esp8266s, 0);
    return DEV_ScanString(DEV_OK, data, size, (char *)data);
}

bool ESP8266_Write(uint8_t *data, size_t size) {
    DEV_SetActStream(esp8266s, 0);
    return DEV_PrintString(DEV_OK, data, size, (char *)data);
}

void ESP8266_Delayms(int ms) {
    delayms_timer(ms);
}



/*****    ESP8266 初始化函数, 将配置存入对象实例    *****/

#define lenof(arr)       (sizeof(arr) / sizeof(*(arr)))
#define ESP8266_BUF_SIZE 256
static char buffer[2][ESP8266_BUF_SIZE];
static const char *taskmsg = "ESP8266";

void ESP8266_WifiInit(ESP8266_Typedef *esp8266, uint8_t mode, const char *ssid, const char *pwd) {
    esp8266->wifi.mode = mode;
    esp8266->wifi.ssid = ssid;
    esp8266->wifi.pwd = pwd;
}

void ESP8266_MqttConnectInit(ESP8266_Typedef *esp8266, char *hostaddr, int hostport) {
    esp8266->mqtt.keepalive = 0;
    esp8266->mqtt.disableCleanSession = false;
    esp8266->mqtt.lwt_topic = "";
    esp8266->mqtt.lwt_msg = "";
    esp8266->mqtt.lwt_qos = 0;
    esp8266->mqtt.lwt_retain = 0;

    esp8266->mqtt.host = hostaddr;
    esp8266->mqtt.port = hostport;
}

void ESP8266_MqttUserInit(ESP8266_Typedef *esp8266, char *clientid, char *username, char *password) {
    esp8266->mqtt.linkId = 0;
    esp8266->mqtt.scheme = 1;
    esp8266->mqtt.certKeyId = 0;
    esp8266->mqtt.CAId = 0;
    esp8266->mqtt.path = "";
    esp8266->mqtt.reconnect = false;

    esp8266->mqtt.clientId = clientid;
    esp8266->mqtt.username = username;
    esp8266->mqtt.password = password;
}

void ESP8266_MqttPubSubInit(ESP8266_Typedef *esp8266) {
    // esp8266->mqtt.publish.buffer = NULL;
    // esp8266->mqtt.publish.num = 0;
    esp8266->mqtt.subscribe.buffer = NULL;
    esp8266->mqtt.subscribe.num = 0;
}

/* 消息的发布暂不做数据持久化处理 */
// int ESP8266_MqttPubTopicInit(ESP8266_Typedef *esp8266,
//                              char *topicPublish[], int qosPublish[], int sizePublish) {
//     if(esp8266->mqtt.publish.num + sizePublish > ESP8266_MAXNUM_TOPIC) {
//         return -1;
//     }
//     for(size_t i = 0; i < sizePublish; i++) {
//         esp8266->mqtt.publish.topic[i] = topicPublish[i];
//         esp8266->mqtt.publish.qos[i] = qosPublish[i];
//         esp8266->mqtt.publish.retain[i] = 0;
//         esp8266->mqtt.publish.num++;
//     }
//     return esp8266->mqtt.publish.num - sizePublish;
// }

int ESP8266_MqttSubTopicInit(ESP8266_Typedef *esp8266, char *topicSubscribe[], int qosSubscribe[],
                             int sizeSubscribe, void *topicBuffer) {
    if(esp8266->mqtt.subscribe.num + sizeSubscribe > ESP8266_MAXNUM_TOPIC) {
        return -1;
    }
    for(size_t i = 0; i < sizeSubscribe; i++) {
        esp8266->mqtt.subscribe.topic[i] = topicSubscribe[i];
        esp8266->mqtt.subscribe.qos[i] = qosSubscribe[i];
        esp8266->mqtt.subscribe.num++;
        memset(esp8266->mqtt.subscribe.requestId[i], 0, sizeof(*esp8266->mqtt.subscribe.requestId));
    }
    esp8266->mqtt.subscribe.buffer = topicBuffer;
    return 0;
}



/*****    ESP8266 功能函数, 根据配置发送相应的串口指令    *****/

static const char escape[] = {',', '"', '\\'};
static char *itoa_s(int value, char *str, size_t size, int8_t base);
static char *strcpy_str_s(char *dest, size_t size, const char *source);
static char *strcpy_int_s(char *dest, size_t size, const int integer);
static char *strcat_str_s(char *dest, size_t size, const char *source);
static char *strcat_int_s(char *dest, size_t size, const int integer);
static char *strcat_s_(char *dest, size_t size, const char *source);

int ESP8266_WifiConnect(ESP8266_Typedef *esp8266) {
    char *str = buffer[0];
    size_t size = sizeof(buffer[0]), len = lenof(buffer[0]);
    memset(str, 0, size);

    strcpy(str, "AT+CWMODE=");
    strcat_int_s(str, len, esp8266->wifi.mode);
    strcat_s_(str, len, "\r\n");
    printf("[%s]%s", taskmsg, str);
    while(!ESP8266_Write((uint8_t *)str, strlen(str) * sizeof(char))) continue;

    strcpy(str, "AT+RST");
    strcat_s_(str, len, "\r\n");
    printf("[%s]%s", taskmsg, str);
    while(!ESP8266_Write((uint8_t *)str, strlen(str) * sizeof(char))) continue;
    ESP8266_Delayms(2500);

    strcpy(str, "AT+CWJAP=");
    strcat_str_s(str, len, esp8266->wifi.ssid);
    strcat_s_(str, len, ",");
    strcat_str_s(str, len, esp8266->wifi.pwd);
    strcat_s_(str, len, "\r\n");
    printf("[%s]%s", taskmsg, str);
    while(!ESP8266_Write((uint8_t *)str, strlen(str) * sizeof(char))) continue;
    ESP8266_Delayms(2500);

    return 0;
}

int ESP8266_MqttUserConfig(ESP8266_Typedef *esp8266) {
    char *str = buffer[0];
    size_t size = sizeof(buffer[0]), len = lenof(buffer[0]);
    memset(str, 0, size);

    strcpy(str, "AT+MQTTUSERCFG=");
    strcat_int_s(str, len, esp8266->mqtt.linkId);
    strcat_s_(str, len, ",");
    strcat_int_s(str, len, esp8266->mqtt.scheme);
    strcat_s_(str, len, ",");
    strcat_str_s(str, len, esp8266->mqtt.clientId);
    strcat_s_(str, len, ",");
    strcat_str_s(str, len, esp8266->mqtt.username);
    strcat_s_(str, len, ",");
    strcat_str_s(str, len, esp8266->mqtt.password);
    strcat_s_(str, len, ",");
    strcat_int_s(str, len, esp8266->mqtt.certKeyId);
    strcat_s_(str, len, ",");
    strcat_int_s(str, len, esp8266->mqtt.CAId);
    strcat_s_(str, len, ",");
    strcat_str_s(str, len, esp8266->mqtt.path);
    strcat_s_(str, len, "\r\n");

    printf("[%s]%s", taskmsg, str);
    while(!ESP8266_Write((uint8_t *)str, strlen(str) * sizeof(char))) continue;

    return 0;
}

int ESP8266_MqttConnectConfig(ESP8266_Typedef *esp8266) {
    char *str = buffer[0];
    size_t size = sizeof(buffer[0]), len = lenof(buffer[0]);
    memset(str, 0, size);

    strcpy(str, "AT+MQTTCONNCFG=");
    strcat_int_s(str, len, esp8266->mqtt.linkId);
    strcat_s_(str, len, ",");
    strcat_int_s(str, len, esp8266->mqtt.keepalive);
    strcat_s_(str, len, ",");
    strcat_int_s(str, len, esp8266->mqtt.disableCleanSession);
    strcat_s_(str, len, ",");
    strcat_str_s(str, len, esp8266->mqtt.lwt_topic);
    strcat_s_(str, len, ",");
    strcat_str_s(str, len, esp8266->mqtt.lwt_msg);
    strcat_s_(str, len, ",");
    strcat_int_s(str, len, esp8266->mqtt.lwt_qos);
    strcat_s_(str, len, ",");
    strcat_int_s(str, len, esp8266->mqtt.lwt_retain);
    strcat_s_(str, len, "\r\n");

    printf("[%s]%s", taskmsg, str);
    while(!ESP8266_Write((uint8_t *)str, strlen(str) * sizeof(char))) continue;

    return 0;
}

int ESP8266_MqttConnect(ESP8266_Typedef *esp8266) {
    char *str = buffer[0];
    size_t size = sizeof(buffer[0]), len = lenof(buffer[0]);
    memset(str, 0, size);

    strcpy(str, "AT+MQTTCONN=");
    strcat_int_s(str, len, esp8266->mqtt.linkId);
    strcat_s_(str, len, ",");
    strcat_str_s(str, len, esp8266->mqtt.host);
    strcat_s_(str, len, ",");
    strcat_int_s(str, len, esp8266->mqtt.port);
    strcat_s_(str, len, ",");
    strcat_int_s(str, len, esp8266->mqtt.reconnect);
    strcat_s_(str, len, "\r\n");

    printf("[%s]%s", taskmsg, str);
    while(!ESP8266_Write((uint8_t *)str, strlen(str) * sizeof(char))) continue;

    ESP8266_MqttPubSubInit(esp8266);

    return 0;
}

int ESP8266_MqttDisconnect(ESP8266_Typedef *esp8266) {
    char *str = buffer[0];
    size_t size = sizeof(buffer[0]), len = lenof(buffer[0]);
    memset(str, 0, size);

    strcpy(str, "AT+MQTTCLEAN=");
    strcat_int_s(str, len, esp8266->mqtt.linkId);
    strcat_s_(str, len, "\r\n");

    printf("[%s]%s", taskmsg, str);
    while(!ESP8266_Write((uint8_t *)str, strlen(str) * sizeof(char))) continue;

    return 0;
}

int ESP8266_MqttPublish(ESP8266_Typedef *esp8266, char *topicPublish, int qosPublish, void *topicBuffer) {
    char *str = buffer[0];
    size_t size = sizeof(buffer[0]), len = lenof(buffer[0]);
    memset(str, 0, size);

    strcpy(str, "AT+MQTTPUB=");
    strcat_int_s(str, len, esp8266->mqtt.linkId);
    strcat_s_(str, len, ",");
    strcat_str_s(str, len, topicPublish);
    strcat_s_(str, len, ",");
    strcat_str_s(str, len, topicBuffer);
    strcat_s_(str, len, ",");
    strcat_int_s(str, len, qosPublish);
    strcat_s_(str, len, ",");
    strcat_int_s(str, len, 0);
    strcat_s_(str, len, "\r\n");

    printf("[%s]%s", taskmsg, str);
    while(!ESP8266_Write((uint8_t *)str, strlen(str) * sizeof(char))) continue;

    return 0;
}

int ESP8266_MqttSubscribe(ESP8266_Typedef *esp8266) {
    char *str = buffer[0];
    size_t size = sizeof(buffer[0]), len = lenof(buffer[0]);

    for(size_t i = 0; i < esp8266->mqtt.subscribe.num; i++) {
        memset(str, 0, size);
        strcpy(str, "AT+MQTTSUB=");
        strcat_int_s(str, len, esp8266->mqtt.linkId);
        strcat_s_(str, len, ",");
        strcat_str_s(str, len, esp8266->mqtt.subscribe.topic[i]);
        strcat_s_(str, len, ",");
        strcat_int_s(str, len, esp8266->mqtt.subscribe.qos[i]);
        strcat_s_(str, len, "\r\n");

        printf("[%s]%s", taskmsg, str);
        while(!ESP8266_Write((uint8_t *)str, strlen(str) * sizeof(char))) continue;
    }

    return 0;
}

int ESP8266_MqttUnsubscribe(ESP8266_Typedef *esp8266) {
    /* 8266有BUG, 会直接断开mqtt连接, 不建议使用 */
    char *str = buffer[0];
    size_t size = sizeof(buffer[0]), len = lenof(buffer[0]);

    for(size_t i = 0; i < esp8266->mqtt.subscribe.num; i++) {
        memset(str, 0, size);
        strcpy(str, "AT+MQTTUNSUB=");
        strcat_int_s(str, len, esp8266->mqtt.linkId);
        strcat_s_(str, len, ",");
        strcat_str_s(str, len, esp8266->mqtt.subscribe.topic[i]);
        strcat_s_(str, len, "\r\n");

        printf("[%s]%s", taskmsg, str);
        while(!ESP8266_Write((uint8_t *)str, strlen(str) * sizeof(char))) continue;
    }

    return 0;
}

int ESP8266_MqttSubRes(ESP8266_Typedef *esp8266) {
    bool rt = false;
    int res = 1;
    char *str = buffer[1];
    size_t size = sizeof(buffer[1]), len = lenof(buffer[1]);
    size_t i;
    size_t length;

    do {
        rt = ESP8266_Read((uint8_t *)str, len);
#if defined(_WIN32) || defined(_WIN64)
        /*****    windows testing part    *****/
        rt = scanf("%s", (uint8_t *)str);
#endif
        if(!rt) { break; }

        /* 匹配指令 */
        strtok(str, ":");
        res = strncmp(str, "+MQTTSUBRECV", sizeof("+MQTTSUBRECV") / sizeof(char));
        if(res != 0) { break; }
        str += strlen(str) + 1;
        /* 匹配 LinkID */
        strtok(str, ",");
        strcpy_int_s(((char *)esp8266->mqtt.subscribe.buffer), len, esp8266->mqtt.linkId);
        res = strncmp(str, (char *)esp8266->mqtt.subscribe.buffer, strlen(((char *)esp8266->mqtt.subscribe.buffer)));
        if(res != 0) { break; }
        str += strlen(str) + 1;
        /* 匹配订阅主题 */
        strtok(str, ",");
        for(i = 0; i < esp8266->mqtt.subscribe.num; i++) {
            strcpy_str_s((char *)esp8266->mqtt.subscribe.buffer, len, esp8266->mqtt.subscribe.topic[i]);
            length = strlen(((char *)esp8266->mqtt.subscribe.buffer));
            if(((char *)esp8266->mqtt.subscribe.buffer)[length - 2] == '#') {
                ((char *)esp8266->mqtt.subscribe.buffer)[length - 2] = '\0';
            }
            length = strlen(((char *)esp8266->mqtt.subscribe.buffer));
            res = strncmp(str, ((char *)esp8266->mqtt.subscribe.buffer), length);
            if(res == 0) { break; }
        }
        if(res != 0) { break; }
        /* 记录带 requestid 的主题 */
        str++;
        strncpy(esp8266->mqtt.subscribe.requestId[i], str, strlen(str) - 1);
        str += strlen(str) + 1;
        /* 取订阅内容至缓存区 */
        strtok(str, ",");
        length = atof(str);
        str += strlen(str) + 1;
        strncpy(esp8266->mqtt.subscribe.buffer, str, length);
        ((char *)esp8266->mqtt.subscribe.buffer)[length] = '\0';
        /* 清空缓存区 */
        memset(buffer[1], 0, size);
        printf("[%s]REQUESTID=%s, PAYLOAD=%s\r\n", taskmsg, esp8266->mqtt.subscribe.requestId[i], ((char *)esp8266->mqtt.subscribe.buffer));

        return 0;
    } while(0);

    return res;
}



/*****    字符串操作的局部函数, 将字符串/整型字符串化后拼接至目标字符串    *****/

static char *itoa_s(int value, char *str, size_t size, int8_t base) {
    int8_t i = 0, j = 0, bit = 0;
    int temp = value;

    if(temp == 0) {
        bit++;
    } else if(temp < 0) {
        str[0] = '-';
        value = -value;
        bit++;
        if(i + 1 >= size) {
            str[i] = '\0';
            return NULL;
        }
        i++;
    }
    while(temp) {
        temp /= base;
        bit++;
    }

    for(i = i; i < bit; i++) {
        temp = value;
        for(j = i; j < bit - 1; j++) {
            temp /= base;
        }
        if(i + 1 >= size) {
            str[i] = '\0';
            return NULL;
        }
        if(temp < 10) {
            str[i] = temp + '0';
        } else if(temp < 36) {
            str[i] = temp - 10 + 'a';
        } else {
            str[i] = temp - 36 + 'A';
        }
        value = value - temp * pow(base, bit - 1 - i);
    }
    str[i] = '\0';

    return str;
}
static char *strcpy_str_s(char *dest, size_t size, const char *source) {
    char *str = dest;

    /* 在字符串头添加双引号 */
    if(str + 1 >= dest + size) {
        *str = '\0';
        return NULL;
    }
    *str = '\"';
    str++;

    /* 合并字符串 */
    for(size_t i = 0; source[i] != '\0'; i++) {
        /* 检测并添加转义符, 8266的转义规则与stdio不同 */
        for(size_t j = 0; j < sizeof(escape) / sizeof(*escape); j++) {
            if(source[i] == escape[j]) {
                if(str + 2 >= dest + size) {
                    *str = '\0';
                    return NULL;
                }
                *str = '\\';
                str++;
                break;
            }
        }
        /* 合并 */
        if(str + 1 >= dest + size) {
            *str = '\0';
            return NULL;
        }
        *str = source[i];
        str++;
    }
    /* 在字符串尾添加双引号 */
    if(str + 1 >= dest + size) {
        *str = '\0';
        return NULL;
    }
    *str = '\"';
    str++;
    /* 合并结束, 添加'\0' */
    if(str + 1 >= dest + size) {
        *str = '\0';
        return NULL;
    }
    *str = '\0';

    return dest;
}
static char *strcpy_int_s(char *dest, size_t size, const int integer) {
    char *str = dest;
    if(itoa_s(integer, str, size, 10) == NULL) {
        return NULL;
    }
    return dest;
}
static char *strcat_str_s(char *dest, size_t size, const char *source) {
    char *str = dest;
    while(*str != '\0') {
        str++;
        size--;
    }
    return strcpy_str_s(str, size, source);
}
static char *strcat_int_s(char *dest, size_t size, const int integer) {
    char *str = dest;
    while(*str != '\0') {
        str++;
        size--;
    }
    return strcpy_int_s(str, size, integer);
}
static char *strcat_s_(char *dest, size_t size, const char *source) {
    if(strlen(dest) + strlen(source) > size) {
        return NULL;
    }
    return strcat(dest, source);
}


#if defined(_WIN32) || defined(_WIN64)
/*****    windows testing part    *****/
/* wifi 连接 */
const char *wifi_ssid = "ECLaboratory";
const char *wifi_password = "SXL236236";
const char *wifi_encryption = "wpawpa2_aes";

/* mqtt 连接 */
static char *mqtt_hostaddr = "2bda7f655e.iot-mqtts.cn-north-4.myhuaweicloud.com";
static int mqtt_hostport = 1883;
/* mqtt 鉴权 */
static char *mqtt_clientid = "6371b5061d98cb0484f3de27_a6-cf-12-ef-60-a9_0_0_2022111405";
static char *mqtt_username = "6371b5061d98cb0484f3de27_a6-cf-12-ef-60-a9";
static char *mqtt_password = "1c6537ee6caaecc2a354ae5d136cd03db97f21370b23529362b867fe3060a64e";
/* mqtt 主题 */
static char *mqtt_topic_report = "$oc/devices/6371b5061d98cb0484f3de27_a6-cf-12-ef-60-a9/sys/properties/report";
static int mqtt_qos_report = 0;
static char *mqtt_topic_cmd = "$oc/devices/6371b5061d98cb0484f3de27_a6-cf-12-ef-60-a9/sys/commands/#";
static int mqtt_qos_cmd = 0;
static char *mqtt_topic_cmdres_prefix = "$oc/devices/6371b5061d98cb0484f3de27_a6-cf-12-ef-60-a9/sys/commands/response";

char buffer_test[ESP8266_BUF_SIZE] = "{\"services\": [{\"service_id\": \"SmokeDetectorBasic\",\"properties\": {\"airTemperature\": 21,\"airHumidity\": 40,\"airCoConcentration\": 33}}]}";

static int main(void) {
    ESP8266_Typedef esp8266;
    ESP8266_WifiInit(&esp8266, 1, wifi_ssid, wifi_password);
    ESP8266_WifiConnect(&esp8266);

    ESP8266_MqttConnectInit(&esp8266, mqtt_hostaddr, mqtt_hostport);
    ESP8266_MqttUserInit(&esp8266, mqtt_clientid, mqtt_username, mqtt_password);

    ESP8266_MqttUserConfig(&esp8266);
    ESP8266_MqttConnectConfig(&esp8266);
    ESP8266_MqttConnect(&esp8266);

    ESP8266_MqttPubSubInit(&esp8266);
    // ESP8266_MqttPubTopicInit(&esp8266, &mqtt_topic_report, &mqtt_qos_report, 1);
    ESP8266_MqttPublish(&esp8266, mqtt_topic_report, mqtt_qos_report, buffer_test);

    ESP8266_MqttSubTopicInit(&esp8266, &mqtt_topic_cmd, &mqtt_qos_cmd, 1, buffer_test);
    ESP8266_MqttSubscribe(&esp8266);
    ESP8266_MqttSubRes(&esp8266);
    ESP8266_MqttUnsubscribe(&esp8266);

    ESP8266_MqttDisconnect(&esp8266);
}

#endif
