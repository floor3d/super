## Proposed database layout

### Database SUPER

**Implant**

Id PRIMARY KEY

FirstConnected TIMESTAMP

IsActive BOOLEAN


**Keylog**

Id PRIMARY KEY

Text STRING

ImplantId FOREIGN KEY
