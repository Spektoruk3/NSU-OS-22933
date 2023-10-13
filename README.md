# 22933


# Первый этап

1. Перейти по ссылке https://github.com/vkinsu/NSU-OS-22932
2. В правом верхнем углу нажать на Fork -> Create new fork...
3. Теперь в профиле есть копия этого репозитория


# Второй этап
Если вы ~~нормальный человек и пользуетесь~~ используете linux то можно пойти таким путем:
1. После того как был создан fork перейдите в свой аккаунт github
2. Затем нажмите на кнопку clone там будет ссылка на ваш репозиторий
3. Далее в терминале пишете следующую команду где ссылка это то что вы скопировали на предыдущем шаге:
`git clone ссылка`
4. теперь на вашей машине есть копия репозитория 


# Третий этап (сложно)

1. На своей машине пишем команду `ssh-keygen -t rsa` первым вопросом будет как назвать файл. Рекомендуется назвать его по-своему, например gitnsu. Пароль ставить не обязательно, можно просто нажать на Enter.
2. Затем у вас появятся два файла gitnsu и gitnsu.pub (тут, название gitnsu просто пример)
3. теперь необходимо добавить ключ в стандартный путь git для этого можно выполнить следующее:
`cp gitnsu ~/.ssh/` и затем
`ssh-add ~/.ssh/gitnsu`


# Третий с половиной этап
Возвращаемся на github.

1. Нажимаем на кнопку справа сверху где есть иконка вашего профиля и заходим в settings
   
![sample](https://github.com/alexmihalyk23/NSU-OS-22930/assets/35634279/d32d8aff-f1b0-4c5d-8c13-91eb713f6e69)

2. Затем нажимаем на кнопку SSH and GPG keys 

![ssh keys](https://github.com/alexmihalyk23/NSU-OS-22930/assets/35634279/637d4072-bcd8-47c3-8a47-c3ea84177114)

3. Нажимаем на кнопку New ssh key
4. На вашей машине открываете gitnsu.pub и копируете весь текст
5. После этого вставляете его в поле ключа на github


В папке с вашим репозиторием находится папка .git, в которой есть файл config в нем необходимо заменить строчку url
на  git@github.com:ваше имя аккаунта/NSU-OS-22930.git
=======
# NSU-OS-22933
>>>>>>> d2c7542fc29151ae17afa9b725cb53f5fb2d6d06
