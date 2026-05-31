INSTALLED_APP = [
    ...
    'mdprojeto'
]


DATABASES = {
    'default': {
        'ENGINE' : 'django.db.sqlite3',
        'NAME' : BASE_DIR / 'db.sqlite3',
    }
}

STATIC_URL = '/static/'