import sys
import time
from selenium import webdriver

driver = webdriver.Chrome('chromedriver')

driver.get("http://192.168.0.1")
driver.find_element_by_name("username").send_keys("root")
driver.find_element_by_name("passwd").send_keys("root")
driver.find_element_by_id("submit_bt").click()

driver.get("http://192.168.0.1/sess-bin/timepro.cgi?tmenu=netconf&smenu=wansetup")
elem = driver.find_element_by_name("hw_dynamic6")
elem.click()
h = int(elem.get_attribute("value"))
if int(sys.argv[1]) != h:
	elem.send_keys(sys.argv[1])
	driver.find_element_by_id("appbtn").click()
	time.sleep(6)
print("vrai")
driver.get("http://192.168.0.1/sess-bin/login_session.cgi?logout=1")
driver.quit()

