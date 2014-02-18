Walkthrough Selenium Executor
=============================
Jenkins tasks and helper scripts to retrieve and play Walkthroughs via Selenium.

Installation
-------------
    ```sh
    # Install Composer
    $ curl -sS https://getcomposer.org/installer | php

    # Install dependencies
    $ php composer.phar install
    ```

Usage
-----

Test logging in:

* Using basic authentication
    ```sh
    $ php wt_selenium.php -w [walkhub url] -u [username] -p [password] status
    Ok.
    ```
* Via OAuth
    ```sh
    $ php wt_selenium.php -w [walkhub url] -k [consumer key] -s [consumer secret] process_queue
    Ok.
    ```

Taking automatic screenshots:

* Simple screenshot queue processing
    ```sh
    $ php wt_selenium.php -w [walkhub url] -u [username] -p [password] process_queue
    .
    ```
* Processing multiple items from the queue
    ```sh
    $ php wt_selenium.php -w [walkhub url] -u [username] -p [password] -l [number of items to process] process_queue
    ..E.E....
    ```
* Change the target browser
    ```sh
    $ php wt_selenium.php -w [walkhub url] -u [username] -p [password] -b [browser string] process_queue
    .
    ```
    
Taking screenshots using saucelabs:

1. Log in to saucelabs
    ```sh
    $ vendor/bin/sauce_config [YOUR_SAUCE_USERNAME] [YOUR_SAUCE_ACCESS_KEY]
    ```
    
2. Extend the saucelabs class when processing the queue
    ```sh
    $ php wt_selenium.php -w [walkhub url] -u [username] -p [password] -e "Sauce\Sausage\WebDriverTestCase" process_queue
    .
    ```
