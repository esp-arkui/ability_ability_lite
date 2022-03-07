## 需求编号
    27611790
###需求描述   
    设备管理-BOX单板管理页提供接口支持以下功能：导入单板，编辑单板，删除单板，新增单板，单板列表，单板搜索
###需求实现
1. 单板管理新增
    - url: post  /ci-portal/environment/equipment/v1/board
    - params : BoardModel in body
2. 单板管理编辑
    - url: put  /ci-portal/environment/equipment/v1/board
    - params : BoardModel in body
3. 单板管理(搜索|列表)
    - url: get  /ci-portal/environment/equipment/v1/board
    - params : QueryModel in query
4. 单板管理删除
    - url: delete  /ci-portal/environment/equipment/v1/board/{boardId}
    - params : String boardId in path
5. 单板管理导入
    - url: post  /ci-portal/environment/equipment/v1/board/excel/upload
    - params : MultipartFile in body
###业务流程
#### 编辑单板，删除单板，新增单板，单板列表，单板搜索
    1. 校验参数
        失败 -> 400 failed 提示信息
    2. 操作数据库
    3. 返回结果
        成功 -> 200 (success|result) 
#### 单板管理导入
    1. 读取excel数据
        失败 -> 400 failed 提示信息
    2. 校验数据
        失败 -> 400 failed 提示信息
    3. 数据入库
    4. 返回结果
        成功 -> 200 success 
###涉及java类
#### board 单板信息表
|  字段  |  数据类型  |  描述  |
|  ---  |  ---  |  --- |
|  id  |  String  |  单板信息的唯一标识  |
|  type  |  String  |  单板类型  |
|  name  |  String  |  单板名称  |
|  sn  |  String  |  SN编号  |
|  version  |  String  |  版本  |
|  level  |  String  |  单板级别(L0、L1、L2)  |
|  box  |  Object  |  管理box  |
|  ip  |  String  |  管理IP  |
|  status  |  int  |  状态(0:正常、1:空闲、2:故障)  |
|  location  |  String  |  物理位置  |
|  port  |  int  |  端口(box的物理接口0~7)  |
|  userName  |  String  |  使用人名称  |
|  createUser  |  String  |  创建人  |
|  createTime  |  String  |  创建时间  |
|  updateUser  |  String  |  更新人  |
|  updateTime  |  String  |  更新时间  |
|  isDelete  |  String  |  逻辑删除(1:逻辑删除)  |

 
#### queryBoardModel 单板信息表
|  字段  |  数据类型  |  描述  |
|  ---  |  ---  |  --- |
|  pageNum  |  int  |  查询的页数  |
|  pageSize  |  int  |  每页显示的行数  |
|  type  |  String  |  单板类型  |
|  level  |  String  |  单板级别(L0、L1、L2)  |
|  status  |  int  |  状态(0:正常、1:空闲、2:故障)  |
|  content  |  String  |  查询的内容(适配name、sn、box、ip、userName)  |

   
