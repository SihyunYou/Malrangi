import os
import time
from datetime import datetime, timedelta

def disable_quickedit():
    if not os.name == 'posix':
        try:
            import msvcrt
            import ctypes
            kernel32 = ctypes.WinDLL('kernel32', use_last_error=True)
            device = r'\\.\CONIN$'
            with open(device, 'r') as con:
                hCon = msvcrt.get_osfhandle(con.fileno())
                kernel32.SetConsoleMode(hCon, 0x0080)
        except Exception as e:
            print('Cannot disable QuickEdit mode! ' + str(e))
            print('.. As a consequence the script might be automatically\
            paused on Windows terminal')

def imprimer_et_commander(flag):
	nom = ""
	if flag & 0b001:
		nom = "urus.exe"
	elif flag & 0b010:
		nom = "boss.exe"
	elif flag & 0b100:
		nom = "compte.exe"

	print("Le temp operé de", nom, " : ", datetime.today())
	os.system(nom)

def réserver_jour(jour_relatif, flags):
	présent = datetime.today()
	futur = datetime(présent.year, présent.month, présent.day)
	futur += timedelta(days=jour_relatif)
	
	while flags:
		flag = flags & 0b001
		if flag:
			if datetime.today() > futur.replace(hour=18, minute=59): # récent > dernier
				imprimer_et_commander(flag)
				flags &= 0b110
		flag = flags & 0b010
		if flag:
			if datetime.today() > futur.replace(hour=0):
				imprimer_et_commander(flag)
				flags &= 0b101
		flag = flags & 0b100
		if flag:
			if datetime.today() > futur.replace(hour=0):
				if flags & 0b011:
					continue
				else:
					imprimer_et_commander(flag)
					flags &= 0b011
		time.sleep(1)

if __name__ == '__main__':
	disable_quickedit()
	réserver_jour(0, 0b001)
	os.system("shutdown -s -t 30")


