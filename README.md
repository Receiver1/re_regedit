# re_regedit
#### Description:
Lightweight class for easy work with the registry.  
Firstly you create object of class and add a exception handler:
```C++
try {
    RegEdit re{RegEdit::Key::KEY_CURRENT_USER, "Software\\test-app"};
    /* 
    *   Instead of constructor, you can use method:
    *   re.createCatalog(RegEdit::Key::KEY_CURRENT_USER, "Software\\test-app");
    */
    
    // etc...
} catch (std::system_error &error) {
    std::cout << "[RegEdit] Exception: " << error.what() << std::endl;
}  
```
#### Available type methods  
String:  
```C++
re.setString("access_token", "new-access-token");
std::string accessToken{re.getString("access_token")};
```
DWORD (unsigned long):
```C++
re.setUlong("expires_in", 123);
unsigned long expiresIn{re.getUlong("expires_in")};
```
Binary (raw data, structures, etc...):
```C++
#pragma pack(push, 1)
struct TestStruct {
  bool a{};
  char b{};
  short c{};
};
#pragma pack(pop)

re.setBinary<TestStruct>("test_struct", {true, 228, 1337});
TestStruct testStruct{re.getBinary<TestStruct>("test_struct")};
```
#### Catalogs
Subkey\`s in this library renamed to catalogs, you can create, open, and remove it\`s.
```C++
// Create catalog, or open if already exists
re.createCatalog(RegEdit::Key::KEY_CURRENT_USER, "Software\\test-app");

// Removes catalog
re.removeCatalog(RegEdit::Key::KEY_CURRENT_USER, "Software\\test-app");
```
