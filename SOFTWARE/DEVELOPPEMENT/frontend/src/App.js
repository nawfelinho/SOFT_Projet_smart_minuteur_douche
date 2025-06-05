import React, { useEffect, useState } from 'react';

function App() {
    const [users, setUsers] = useState([]);
    const [douches, setDouches] = useState([]);

    useEffect(() => {
        fetch("http://localhost:8080/users")
            .then(res => res.json())
            .then(setUsers)
            .catch(console.error);

        fetch("http://localhost:8080/douches")
            .then(res => res.json())
            .then(setDouches)
            .catch(console.error);
    }, []);

    const getUserById = (id) => users.find(user => user.id === id);

    return (
        <div style={{ padding: '2rem' }}>
            <h1>Liste des utilisateurs</h1>
            <ul>
                {users.map(user => (
                    <li key={user.id}>
                        <strong>{user.nom.toUpperCase()} {user.prenom}</strong> — Temps total : {user.tempsTotal} sec
                    </li>
                ))}
            </ul>

            <h1 style={{ marginTop: '3rem' }}>Historique des douches</h1>
            {douches.length === 0 ? (
                <p>Aucune douche enregistrée.</p>
            ) : (
                <table border="1" cellPadding="10" style={{ borderCollapse: 'collapse', marginTop: '1rem' }}>
                    <thead>
                    <tr>
                        <th>Utilisateur</th>
                        <th>Date début</th>
                        <th>Date fin</th>
                        <th>Durée (s)</th>
                        <th>Temps dépassé (s)</th>
                    </tr>
                    </thead>
                    <tbody>
                    {douches.map((douche, index) => {
                        const user = getUserById(douche.user?.id);
                        return (
                            <tr key={index}>
                                <td>{user ? `${user.prenom} ${user.nom}` : 'Utilisateur inconnu'}</td>
                                <td>{douche.dateDebut}</td>
                                <td>{douche.dateFin}</td>
                                <td>{douche.duree}</td>
                                <td style={{ color: douche.tempsDepasse > 0 ? 'red' : 'black' }}>
                                    {douche.tempsDepasse}
                                </td>
                            </tr>
                        );
                    })}
                    </tbody>
                </table>
            )}
        </div>
    );
}

export default App;
