import requests
from colorama import Fore, Style

ROOT = "http://localhost:8080"

def test_health():
    r = requests.get(f"{ROOT}/health")
    assert r.status_code == 200
    print(Fore.GREEN + "Health \t\t- passed" + Style.RESET_ALL)

def test_404():
    r = requests.get(f"{ROOT}/doesnotexist")
    assert r.status_code == 404
    print(Fore.GREEN + "Not Found \t- passed" + Style.RESET_ALL)

if __name__ == "__main__":
    print()
    print(Fore.YELLOW + "----- Running tests -----" + Style.RESET_ALL)
    test_health()
    test_404()
    print(Fore.YELLOW + "🎉 All tests passed" + Style.RESET_ALL)