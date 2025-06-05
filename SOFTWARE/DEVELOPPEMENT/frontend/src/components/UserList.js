import React, { useEffect, useState } from 'react';
import { useNavigate } from 'react-router-dom';

function UserList() {
    const [users, setUsers] = useState([]);
    const [nom, setNom] = useState('');
    const [prenom, setPrenom] = useState('');
    const [message, setMessage] = useState('');
    const navigate = useNavigate();

    useEffect(() => {
        fetch('http://localhost:8080/users')
            .then(res => res.json())
            .then(data => setUsers(data.filter(u => u !== null)))
            .catch(err => console.error('Erreur de chargement des utilisateurs :', err));
    }, []);

    const handleUserClick = (user) => {
        // Envoi du nom au backend BLE
        fetch('http://localhost:8080/ble/sendUserToEsp32', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ username: user.nom })
        })
            .then(res => res.json())
            .then(() => {
                navigate(`/user/${user.id}`);
            })
            .catch(err => console.error('Erreur envoi ESP32 :', err));
    };

    const handleCreateUser = () => {
        const trimmedNom = nom.trim();
        const trimmedPrenom = prenom.trim();
        if (trimmedNom.length === 0 || trimmedPrenom.length === 0) {
            setMessage("Champs vides non autorisés");
            return;
        }

        if (trimmedNom.length > 20 || trimmedPrenom.length > 20) {
            setMessage("Nom et prénom doivent contenir 20 caractères max");
            return;
        }

        const exists = users.some(u =>
            u &&
            u.nom.toLowerCase() === trimmedNom.toLowerCase() &&
            u.prenom.toLowerCase() === trimmedPrenom.toLowerCase()
        );

        if (exists) {
            setMessage("Cet utilisateur existe déjà");
            return;
        }

        fetch('http://localhost:8080/users', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ nom: trimmedNom.toUpperCase(), prenom: trimmedPrenom })
        })
            .then(res => res.json())
            .then(() => {
                setNom('');
                setPrenom('');
                setMessage("Utilisateur créé !");
                return fetch('http://localhost:8080/users');
            })
            .then(res => res.json())
            .then(data => setUsers(data.filter(u => u !== null)))
            .catch(err => setMessage("Erreur création utilisateur"));
    };

    return (
        <div>
            <h2>Veuillez choisir un utilisateur :</h2>
            {users.map(user => (
                <button
                    key={user.id}
                    onClick={() => handleUserClick(user)}
                    style={{ margin: '5px' }}
                >
                    {user.prenom} {user.nom}
                </button>
            ))}

            <hr />
            <h3>Créer un nouvel utilisateur :</h3>
            <input
                type="text"
                placeholder="Prénom"
                value={prenom}
                onChange={e => setPrenom(e.target.value)}
                maxLength="20"
            />
            <input
                type="text"
                placeholder="Nom"
                value={nom}
                onChange={e => setNom(e.target.value)}
                maxLength="20"
            />
            <button onClick={handleCreateUser}>Créer</button>

            {message && <p style={{ color: message.includes("créé") ? "green" : "red" }}>{message}</p>}
        </div>
    );
}

export default UserList;
