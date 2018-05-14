# Current Tech:  
![](https://s3.amazonaws.com/iot-course/aws.svg)


## Features
- Authentication with Amplify/Cognito
- GraphQL Subscriptions with VictoryJS Visualizations
- IoT with FreeRTOS and ST32
- Graph DB with Neptune

##  Tree 
```
...
├── cloud ☁️
│   ├── resources
│   │   ├── appsync.yml 🔁
│   │   ├── dynamodb.yml
│   │   └── iam.yml
│   ├── schema.gql 
│   ├── serverless.yml 
│   └── utils
│ 
├── device 🤖
│   ├── config
│   │   └── FreeRTOSConfig.h 🚀
│   ├── lib
│   │   ├── FreeRTOS
│   │   ├── bufferpool
│   │   ├── crypto
│   │   ├── pkcs11
│   │   ├── secure_sockets
│   │   ├── tls
│   │   ├── utils
│   │   └── wifi
│   └── src
│       ├── main.c
│       ├── main.h
│       └── vendor
│ 
└── web 🖥
    ├── index.js ⚛️
    ├── src
    │   ├── App.js
    │   ├── components
    │   └── pages
    ├── web
    │   ├── FontAwesome.ttf
    │   └── index.html
    └── webpack.config.js
```


