<?php

function display_usercv ($user_id) {
    global $user;
    $content = array();

    if ($user_id == $user->uid || user_has_role(3, user_load($user->uid))) {
        $content['edit_button'] = array(
            '#markup' => '<br><a class="btn" href="' . base_path() . 'user/' . $user_id . '/usercv/edit">' . t('Editieren') . '</a>',
        );
    }

    return $content;
}