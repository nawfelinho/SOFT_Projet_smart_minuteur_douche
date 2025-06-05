import React, { useEffect, useState } from 'react';
import { useNavigate } from 'react-router-dom';

function UserList() {
    const [users, setUsers] = useState([]);
    const navigate = useNavigate();

    useEffect(() => {
        fetch('http://localhost:8080/users')
            .then(response => response.json())
            .then(data => setUsers(data))
            .catch(error => console.error('Erreur de chargement des utilisateurs', error));
    }, []);

    const handleClick = (id) => {
        navigate(`/user/${id}`);
    };

    return (
        <div>
            <h2>Veuillez choisir un utilisateur :</h2>
            {users.map(user => (
                <button key={user.id} onClick={() => handleClick(user.id)} style={{ margin: '5px' }}>
                    {user.nom.toUpperCase()} {user.prenom}
                </button>
            ))}
        </div>
    );
}

export default UserList;
