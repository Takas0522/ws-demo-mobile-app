package com.example.webapi.repository;

import com.example.webapi.entity.User;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.Optional;

/**
 * ユーザーリポジトリ
 * 
 * ユーザーエンティティのデータアクセス
 */
@Repository
public interface UserRepository extends JpaRepository<User, Long> {
    
    /**
     * ログインIDでユーザーを検索
     * 
     * @param loginId ログインID
     * @return ユーザー（Optional）
     */
    Optional<User> findByLoginId(String loginId);
    
    /**
     * ログインIDの存在確認
     * 
     * @param loginId ログインID
     * @return 存在する場合true
     */
    boolean existsByLoginId(String loginId);
}
