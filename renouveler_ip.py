import time
from selenium import webdriver

driver = webdriver.Chrome('chromedriver')

driver.get("http://192.168.0.1")
driver.find_element_by_name("username").send_keys("root")
driver.find_element_by_name("passwd").send_keys("root")
driver.find_element_by_id("submit_bt").click()

driver.get("http://192.168.0.1/sess-bin/timepro.cgi?tmenu=netconf&smenu=wansetup")
elem = driver.find_element_by_name("hw_dynamic6")
h = int(elem.get_attribute("value"))
p = 0
if h >= 99:
	p = 10
else:
	p = h + 1
elem.click()
elem.send_keys(p)
driver.find_element_by_id("appbtn").click()

time.sleep(10)
driver.get("http://192.168.0.1/sess-bin/login_session.cgi?logout=1")
driver.quit()

